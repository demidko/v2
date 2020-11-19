#include "Compressor.h"
#include "Parser.h"
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>

namespace {
  /**
   * Функция сохраняет термированные url'ы в файл и возвращает словарь сопоставляющий термы в id
   */
  std::tuple<std::filesystem::path, std::unordered_map<std::string, unsigned long>>
  prepareUrlsAndTerms(std::istream &input) {
    // Файл с термированными url
    auto termedUrlsFilename =
      std::filesystem::temp_directory_path() /
      std::filesystem::path("v2-urls-buffer-" + std::to_string(std::time(nullptr)) + ".tmp");
    // В один проход по потоку логов заполняем файл термированными url
    std::ofstream urlTermsOutput(termedUrlsFilename);
    // И генерируем словарь термов с частотой
    std::unordered_map<std::string, unsigned long> termsMap;
    for (std::string buf; std::getline(input, buf);) {
      auto request = Parser::extract(buf, 23);
      int first = request.find("//");
      int last = request.find_first_of("?\"");
      if (first == -1 || last == -1) {
        // значит это не url, а что нибудь вроде "-" или "5aef8db%2FT0w16Io%2FmYOshHdtzlxGA0ab"
        // (таких значений примерно треть на 100 000 записей)
        continue;
      }
      auto len = last - first;
      auto url = request.substr(first, len);
      std::cout << url << " (" << last << " - " << first << " = " << len << ")" << std::endl;
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

void Compressor::compress(std::istream &in) {
  auto[termsFilename, termsMap] = prepareUrlsAndTerms(in);
  std::cout << termsFilename;
  /*std::ifstream termStream(termsFilename);
  for (std::string buf; std::getline(termStream, buf);) {
    std::cout << buf << std::endl;
  }*/
}