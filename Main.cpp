#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <map>
#include <CLI/App.hpp>


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
  CLI::App v2("fast access logs compressor/decompressor", "v2");
  cxxopts::Options options("v2", "fast log decompression");
  options.add_options()
    ("d,decompress", "uncompress file", cxxopts::value<std::string>())
    ("d,decompress", "uncompress file");
  auto params = options.parse(argc, argv);
  for (const auto &x: params.arguments()) {
    std::cout << x.key() << " -> " << x.value();
  }
}