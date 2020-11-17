#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <map>


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

  using namespace std::string_view_literals;

  if (argc == 1) {
    return compress(std::cin);
  }

  if (argc == 2) {
    if (argv[1] == "-d"sv) {
      return decompress(std::cin);
    }
    return compress(std::ifstream(argv[1]));
  }

  if (argc == 3 && std::string_view("-d") == argv[1]) {
    return decompress(argv[2]);
  }

  std::cerr << "Invalid options. Usage:\n"
               "  v2    [filename]      for compression\n"
               "  v2 -d [filename]      for decompression" << std::endl;

  return 1;
}