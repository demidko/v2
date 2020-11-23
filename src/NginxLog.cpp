#include <NginxLog.h>
#include <unordered_map>
#include <map>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <VlqOstream.h>
#include <list>
#include <sstream>

std::istringstream &parse(const std::string &text) {
  static std::istringstream lineParser;
  lineParser.clear();
  lineParser.str(text);
  return lineParser;
}

std::string extract(const std::string &fromText, int wordAtIndex) {
  return *std::next(std::istream_iterator<std::string>(parse(fromText)), wordAtIndex);
}

static std::unordered_map<std::string, uintmax_t>
buildTerms(const std::string &logFilename, const std::string &urlsBuffer) {
  std::ifstream log(logFilename);
  // В один проход по потоку логов заполняем файл термированными url
  std::ofstream termedUrls(urlsBuffer);
  // И генерируем словарь термов с частотой
  std::unordered_map<std::string, uintmax_t> termsMap;
  for (std::string buf; std::getline(log, buf);) {
    auto request = extract(buf, 23);
    auto first = request.find("//");
    auto last = request.find_first_of("?\"", first);
    if (first == -1 || last == -1) {
      // значит это не url, а что нибудь вроде "-" или "5aef8db%2FT0w16Io%2FmYOshHdtzlxGA0ab"
      // (таких значений примерно треть на 100 000 записей)
      continue;
    }
    auto url = request.substr(first + 2, last - first - 2);
    for (auto &terms = parse(url); std::getline(terms, buf, '/');) {
      if (!buf.empty()) {
        ++termsMap[buf];
        termedUrls << buf << ' ';
      }
    }
    termedUrls << '\n';
  }
  // Сортируем термы по возрастанию частоты
  std::multimap<uintmax_t, std::reference_wrapper<const std::basic_string<char>>> frequencyMap;
  for (auto &&[term, frequency]: termsMap) {
    frequencyMap.emplace(frequency, std::ref(term));
  }
  // Заменяем частоту на идентификаторы в несортированном словаре термов
  auto identifier = std::size(termsMap) + 1u;
  for (auto &&[_, term]: frequencyMap) {
    termsMap[term] = --identifier;
  }
  return termsMap;
}


void NginxLog::compress(const std::string &logFilename) {
  auto orderedTermsBuffer = logFilename + ".v2.terms";
  auto termsMap = buildTerms(logFilename, orderedTermsBuffer);
  std::ofstream v2compressed(logFilename + ".v2", std::ios::binary);
  auto mapLen = termsMap.size();
  v2compressed.write(reinterpret_cast<char *>(&mapLen), sizeof(mapLen));
  char empty = ' ';
  v2compressed.write(&empty, 1);
  for (auto &&[k, _]: termsMap) {
    v2compressed.write(k.data(), k.size());
    v2compressed.write(&empty, 1);
  }
  VlqOstream vlqEncoder(v2compressed);
  for (auto &&[_, v]: termsMap) {
    vlqEncoder << v;
  }
  std::ifstream orderedTermsStream(orderedTermsBuffer);
  for (std::string buf; std::getline(orderedTermsStream, buf);) {
    std::list<uint64_t> url;
    for (auto &terms = parse(buf); std::getline(terms, buf, ' ');) {
      url.push_back(termsMap[buf]);
    }
    vlqEncoder << url.size();
    for (auto &&word: url) {
      vlqEncoder << word;
    }
  }
  std::filesystem::remove(orderedTermsBuffer);
}