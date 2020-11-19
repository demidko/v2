#include "Compressor.h"
#include "Parser.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <map>
#include <string>
#include <sstream>

namespace {
  /**
   * Функция сохраняет термированные url'ы в файл и возвращает словарь сопоставляющий термы в id
   */
  std::tuple<std::filesystem::path, std::unordered_map<std::string, unsigned long>>
  prepareUrlsAndTerms(std::istream &input) {
    // Файл с термированными url
    auto termedUrlsFilename = std::filesystem::temp_directory_path()
      .append("v2-urls-buffer-")
      .append(std::to_string(std::time(nullptr)))
      .append(".tmp");
    // В один проход по потоку логов заполняем файл термированными url
    std::ofstream urlTermsOutput(termedUrlsFilename);
    // И генерируем словарь термов с частотой
    std::unordered_map<std::string, unsigned long> termsMap;
    u_long errors = 0;
    for (std::string buf; std::getline(input, buf);) {
      auto request = Parser::extract(buf, 23);
      auto first = request.find("//");
      auto last = request.find_first_of("?\"");
      if (first == -1 || last == -1) {
        // значит это не url, а что нибудь вроде "-" или "5aef8db%2FT0w16Io%2FmYOshHdtzlxGA0ab"
        // (таких значений примерно треть на 100 000 записей)
        continue;
      }
      auto url = request.substr(first, last - first);
      for (auto &terms = Parser::parse(url); std::getline(terms, buf, '/');) {
        if (!buf.empty()) {
          ++termsMap[buf];
          urlTermsOutput << buf << ' ';
        }
      }
      urlTermsOutput << '\n';
    }
    // Сортируем термы по возрастанию частоты
    std::multimap<unsigned long, std::reference_wrapper<const std::basic_string<char>>> frequencyMap;
    for (auto &&[term, frequency]: termsMap) { frequencyMap.emplace(frequency, std::ref(term)); }
    // Заменяем частоту на идентификаторы в несортированном словаре термов
    auto identifier = std::size(termsMap) + 1ul;
    for (auto &&[_, term]: frequencyMap) {
      termsMap[term] = --identifier;
    }
    // Перемещаем путь и словарь термов через кортеж на уровень выше
    return {std::move(termedUrlsFilename), std::move(termsMap)};
  }
}

void Compressor::compress(std::istream &rowStream) {
  auto[termsFilename, termsMap] = prepareUrlsAndTerms(rowStream);
  std::ifstream termStream(termsFilename);

  for (std::string buf; std::getline(termStream, buf);) {
    std::cout << buf << std::endl;
  }
}