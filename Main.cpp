#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <map>
#include <optional>
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>


void compress(std::istream &in) {

  std::unordered_map<std::string, int> unsortedFrequency;
  for (std::string buf; std::getline(in, buf);) {
    std::istringstream line(buf);
    auto fullUrl = std::next(std::istream_iterator<std::string>(line), 9);
    auto shortUrl = std::istringstream(fullUrl->substr(0, fullUrl->find('?')));
    for (std::string term; std::getline(shortUrl, term, '/');) {
      ++unsortedFrequency[term];
    }
  }

  std::multimap<int, std::string> sortedFrequency;
  std::transform(
    unsortedFrequency.cbegin(),
    unsortedFrequency.cend(),
    std::inserter(sortedFrequency, sortedFrequency.begin()),
    [](auto &&p) { return std::pair{std::move(p.second), p.first}; }
  );

  for (auto &&[k, v]: sortedFrequency) {
    std::cout << k << " -> " << v << "\n";
  }
  std::cout << std::flush;
}


void decompress(std::istream &in) {
  throw std::runtime_error("not implemented yet");
}


template<typename F>
std::function<bool(const std::vector<std::string> &)> bind(F &&handler) {
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
  CLI::App v2("Farpost access logs compressor/decompressor", "v2");
  v2.add_option("-c,--compress", bind(compress), "Compress raw log files [filename, ...]")
    ->expected(0, INT_MAX);
  v2.add_option("-d,--decompress", bind(decompress), "Read compressed v2 files [filename, ...]")
    ->expected(0, INT_MAX);
  CLI11_PARSE(v2, argc, argv)
}