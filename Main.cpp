#include <iostream>
#include <string_view>

using namespace std::literals::string_view_literals;

int main(int argc, char **argv) {
  if (argc == 0) {
    // компрессия
    return 0;
  }
  if (argc == 1 && "-d"sv == argv[0]) {
    // декомпрессия
    return 0;
  }
  std::cerr << "Invalid options. Usage examples:\n"
               "  v2 filename         for compression\n"
               "  v2 -d filename      for decompression" << std::endl;
  return 1;
}