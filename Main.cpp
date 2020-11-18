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
 * Переиспользуем один и тот же объект из статической памяти,
 * чтобы не тратить время на размещение при конструировании каждый раз.
 * Буффер очищается при каждом запросе на переиспользование.
 * @param text строка для парсинга
 * @return возвращаемый поток легко можно парсить по разделителям при помощи getline(stream, buf, delimeter)
 */
std::istringstream &createParserFor(const std::string &text) {
  static std::istringstream lineParser;
  lineParser.clear();
  lineParser.str(text);
  return lineParser;
}


std::string extractUrlFrom(const std::string &request) {
  auto first = request.find("//");
  auto last = request.find_first_of("?\"");
  return request.substr(first, last - first);
}

std::string extractWordFrom(const std::string &text, int withIndex) {
  return *std::next(std::istream_iterator<std::string>(createParserFor(text)), withIndex);
}

/**
 * @param in поток данных из лога
 * @return поток термов + словарь с их идентификаторами по частоте
 */
std::tuple<std::ifstream, std::map<std::string, unsigned long>> prepareUrlsAndTerms(std::istream &in) {
  auto urlsFilename = std::filesystem::temp_directory_path()
    .append("v2-urls-buffer-")
    .append(std::to_string(std::time(nullptr)))
    .append(".tmp");
  std::unordered_map<std::string, int> unsortedFrequency;
  std::string textBuffer;
  for (std::ofstream urlsCollector(urlsFilename); std::getline(in, textBuffer);) {
    auto request = *std::next(std::istream_iterator<std::string>(createParserFor(textBuffer)), 23);
    auto url = extractUrlFrom(request);
    urlsCollector << url << '\n';
    for (auto &urlTerms = createParserFor(url); std::getline(urlTerms, textBuffer, '/');) {
      if (!textBuffer.empty()) {
        ++unsortedFrequency[textBuffer];
      }
    }
  }
  // Сортируем термы по возрастанию частоты
  std::map<int, std::string> sortedFrequency;
  std::transform(
    unsortedFrequency.cbegin(),
    unsortedFrequency.cend(),
    std::inserter(sortedFrequency, sortedFrequency.begin()),
    [](auto &&p) { return std::pair(p.second, std::move(p.first)); }
  );
  // Генерируем словарь: терм -> чем чаще встречается терм тем меньший порядковый id он получит
  std::map<std::string, unsigned long> terms;
  std::transform(
    sortedFrequency.cbegin(),
    sortedFrequency.cend(),
    std::inserter(terms, terms.begin()),
    [id = std::size(sortedFrequency) + 1](auto &&p) mutable { return std::pair(std::move(p.second), --id); }
  );
  return {std::ifstream(urlsFilename), std::move(terms)};
}

void compress(std::istream &in) {
  auto[urls, terms] = prepareUrlsAndTerms(in);
  for (auto &&[k, v]: terms) std::cout << k << " -> " << v << "\n";
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