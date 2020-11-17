#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <map>


int compress(std::string_view filename) {

  std::unordered_map<std::string, int> unsortedFrequency;
  std::ifstream file(filename);
  for (std::string buf; std::getline(file, buf);) {
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

  std::unordered_map<std::string, int> terms;
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
  return 0;
}

int decompress(std::string_view filename) {
  throw std::runtime_error("not implemented yet");
}

int main(int argc, char **argv) {

  if (argc == 2) {
    return compress(argv[1]);
  }

  if (argc == 3 && std::string_view("-d") == argv[1]) {
    return decompress(argv[2]);
  }

  std::cerr << "Invalid options. Usage:\n"
               "  v2    [filename]      for compression\n"
               "  v2 -d [filename]      for decompression" << std::endl;

  return 1;
}