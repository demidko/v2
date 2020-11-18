#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <CLI/App.hpp>
#include <tuple>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

/**
 * Переиспользуем для парсинга строк один и тот же поток из статической памяти,
 * чтобы не тратить время на размещение и выделение памяти при конструировании каждый раз.
 * @param text строка для парсинга
 * @return возвращаемый поток легко можно парсить по разделителям при помощи getline(stream, buf, delimeter)
 */
std::istringstream &stream(const std::string &text) {
  static std::istringstream lineParser;
  lineParser.clear();
  lineParser.str(text);
  return lineParser;
}

/**
 * Функция извлекает слово из текста разделенного пробелами
 */
std::string extractWord(const std::string &text, int i) {
  return *std::next(std::istream_iterator<std::string>(stream(text)), i);
}

/**
 * Функция парсит url из строки nginx логов
 */
std::istringstream &parseUrlTerms(const std::string &logLine) {
  auto request = extractWord(logLine, 23);
  auto first = request.find("//");
  auto last = request.find_first_of("?\"");
  return stream(request.substr(first, last - first));
}

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
  for (std::string buf; std::getline(input, buf);) {
    for (auto &terms = parseUrlTerms(buf); std::getline(terms, buf, '/');) {
      if (!buf.empty()) {
        ++termsMap[buf];
        urlTermsOutput << buf << ' ';
      }
    }
    urlTermsOutput << '\n';
  }
  // Сортируем термы по возрастанию частоты
  std::multimap<unsigned long, std::string &> frequencyMap;
  for (auto &&[term, frequency]: termsMap) {
    auto &termRef = term;
    frequencyMap.emplace(frequency, const_cast<std::string &>(termRef));
  }
  // Заменяем частоту на идентификаторы в несортированном словаре термов
  auto identifier = std::size(termsMap) + 1ul;
  for (auto &&[_, term]: frequencyMap) { termsMap[term] = --identifier; }
  // Перемещаем путь и словарь термов через кортеж на уровень выше
  return {std::move(termedUrlsFilename), std::move(termsMap)};
}


void compress(std::istream &rowStream) {
  auto[termsFilename, termsMap] = prepareUrlsAndTerms(rowStream);
  std::ifstream termStream(termsFilename);
  for (std::string buf; std::getline(termStream, buf);) {
    std::cout << buf << std::endl;
  }
}

void decompress(std::istream &in) {
  throw std::runtime_error("not implemented yet");
}

template<typename F>
std::function<bool(const std::vector<std::string> &)> inputTo(F &&handler) {
  return [&handler](auto &&files) {
    if (files.empty()) {
      handler(std::cin);
      return true;
    }
    for (auto &&file: files) {
      std::ifstream stream(file);
      handler(stream);
    }
    return true;
  };
}

int main(int argc, char **argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  CLI::App v2("Farpost access logs compressor/decompressor", "v2");
  v2.add_option("-c,--compress", inputTo(compress), "Compress raw log files [filename, ...]")
    ->expected(0, INT_MAX);
  v2.add_option("-d,--decompress", inputTo(decompress), "Read compressed v2 files [filename, ...]")
    ->expected(0, INT_MAX);
  CLI11_PARSE(v2, argc, argv)
}