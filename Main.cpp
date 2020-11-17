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


template<typename Input>
bool compress(Input &&in) { // r&l-value universal ref

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

  std::map<std::string, int> terms;
  std::transform(
    sortedFrequency.cbegin(),
    sortedFrequency.cend(),
    std::inserter(terms, terms.begin()),
    [id = 0](auto &&p) mutable { return std::pair{std::move(p.second), ++id}; }
  );

  for (auto[term, id]: terms) {
    std::cout << term << " -> " << id << '\n';
  }
  std::cout << std::flush;
  return true;
}

template<typename Input>
bool decompress(Input &&in) { // r&l-value universal ref
  throw std::runtime_error("not implemented yet");
}

int main(int argc, char **argv) {
  CLI::App v2{"fast access logs compressor/decompressor", "v2"};
  v2.add_option(
    "-d,--decompress",
    [](const std::vector<std::string> &compressedLogFiles) {
      std::cout << "DCALL\n";
      for (const auto &i: compressedLogFiles) {
        std::cout << i << std::endl;
      }
      return true;
    },
    "Read compressed log"
  )->expected(0, INT_MAX);
  CLI11_PARSE(v2, argc, argv);
}