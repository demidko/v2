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
#include <cstdlib>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>


std::istringstream &parserOf(const std::string &str) {
  static std::istringstream lineParser;
  lineParser.clear();
  lineParser.str(str);
  return lineParser;
}

std::map<std::string, unsigned long> buildTermsMap(std::istream &in) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  std::ofstream urlsOut(std::tmpnam(nullptr));
#pragma clang diagnostic pop
  // Сохраняем частоту термов
  std::unordered_map<std::string, int> unsortedFrequency;
  for (std::string buf; std::getline(in, buf);) {
    auto &lineWords = parserOf(buf);
    auto urlWithParams = std::next(std::istream_iterator<std::string>(lineWords), 23);
    auto urlOnly = urlWithParams->substr(0, urlWithParams->find('?'));
    urlsOut << urlOnly << '\n';
    auto &urlTerms = parserOf(urlOnly);
    for (std::string term; std::getline(urlTerms, term, '/');) {
      if (!term.empty()) {
        ++unsortedFrequency[term];
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
  return terms;
}

void compress(std::istream &in) {


  for (std::string buf; std::getline(in, buf);) {
    std::cout << buf << std::endl;
  }
  std::cout << "ALL LINES WRITED\n";

  std::string buf;

  std::getline(in, buf);
  std::cout << buf << std::endl;
}


void decompress(std::istream &in) {
  throw std::runtime_error("not implemented yet");
}


template<typename F>
std::function<bool(const std::vector<std::string> &)> by(F &&handler) {
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
  CLI::App v2("Farpost access logs compressor/decompressor", "v2");
  v2.add_option("-c,--compress", by(compress), "Compress raw log files [filename, ...]")
    ->expected(0, INT_MAX);
  v2.add_option("-d,--decompress", by(decompress), "Read compressed v2 files [filename, ...]")
    ->expected(0, INT_MAX);
  CLI11_PARSE(v2, argc, argv)
}