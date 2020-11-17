#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <regex>
#include <unordered_map>

using namespace std::string_view_literals;


int compress(std::string_view filename) {
  std::ifstream file(filename);
  std::unordered_map<std::string, int> frequency;
  for (std::string buf; std::getline(file, buf);) {
    std::istringstream line(buf);
    auto fullUrl = std::next(std::istream_iterator<std::string>(line), 9);
    auto shortUrl = std::istringstream(fullUrl->substr(0, fullUrl->find('?')));
    for (std::string term; std::getline(shortUrl, term, '/');) {
      ++frequency[term];
    }
  }
  for (auto[term, frequency]: frequency) {
    std::cout << term << " -> " << frequency << '\n';
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
  if (argc == 3 && "-d"sv == argv[1]) {
    return decompress(argv[2]);
  }
  std::cerr << "Invalid options. Usage:\n"
               "  v2    [filename]      for compression\n"
               "  v2 -d [filename]      for decompression" << std::endl;
  return 1;
}