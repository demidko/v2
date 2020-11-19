#include "Compressor.h"
#include "Parser.h"
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>

static std::unordered_map<std::string, unsigned long>
buildTerms(std::string_view logFilename, std::string_view urlsBuffer) {
  std::ifstream log(logFilename);
  // В один проход по потоку логов заполняем файл термированными url
  std::ofstream termedUrls(urlsBuffer);
  // И генерируем словарь термов с частотой
  std::unordered_map<std::string, unsigned long> termsMap;
  for (std::string buf; std::getline(log, buf);) {
    auto request = Parser::extract(buf, 23);
    auto first = request.find("//");
    auto last = request.find_first_of("?\"", first);
    if (first == -1 || last == -1) {
      // значит это не url, а что нибудь вроде "-" или "5aef8db%2FT0w16Io%2FmYOshHdtzlxGA0ab"
      // (таких значений примерно треть на 100 000 записей)
      continue;
    }
    auto url = request.substr(first + 2, last - first - 2);
    for (auto &terms = Parser::parse(url); std::getline(terms, buf, '/');) {
      if (!buf.empty()) {
        ++termsMap[buf];
        termedUrls << buf << ' ';
      }
    }
    termedUrls << '\n';
  }
  // Сортируем термы по возрастанию частоты
  std::multimap<unsigned long, std::reference_wrapper<const std::basic_string<char>>> frequencyMap;
  for (auto &&[term, frequency]: termsMap) {
    frequencyMap.emplace(frequency, std::ref(term));
  }
  // Заменяем частоту на идентификаторы в несортированном словаре термов
  auto identifier = std::size(termsMap) + 1ul;
  for (auto &&[_, term]: frequencyMap) {
    termsMap[term] = --identifier;
  }
  return termsMap;
}


void Compressor::compress(const std::string &logFilename) {


  auto orderedTermsBuffer = logFilename + ".v2.terms";
  auto termsMap = buildTerms(logFilename, orderedTermsBuffer);

  std::ofstream compressedUrls(logFilename + ".v2", std::ios::binary);
  auto mapLen = termsMap.size();
  compressedUrls.write(reinterpret_cast<char *>(&mapLen), sizeof(mapLen));
  char empty; // поскольку стандарт ничего не говорит о \0 в конце char*, страхуемся
  for (auto &&[term, id]: termsMap) {
    compressedUrls.write(&empty, 1);
    compressedUrls.write(term.data(), termsMap.size());
    compressedUrls.write(&empty, 1);
    compressedUrls.write(reinterpret_cast<char *>(&id), sizeof(id));
  }
  compressedUrls.write(&empty, 1);
  std::ifstream orderedTermsStream(orderedTermsBuffer);
  for (std::string buf; std::getline(orderedTermsStream, buf);) {
    for (auto &terms = Parser::parse(buf); std::getline(terms, buf, ' ');) {
      compressedUrls.write(buf.data(), buf.size());
      compressedUrls.write(&empty, 1);
    }
  }
  orderedTermsStream.close(); // здесь не полагаемся на автоматический вызов деструктора т к нам еще удалять нужно
  std::filesystem::remove(orderedTermsBuffer);
}