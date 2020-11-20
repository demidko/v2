#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include "Compressor.h"
#include "Decompressor.h"
#include "Handler.h"
#include "Vlq.h"


int main(int argc, char **argv) {

  using namespace Vlq;
  for (std::string buf; std::getline(std::cin, buf);) {
    auto n = std::stoul(buf);
    std::bitset<sizeof(long) * 8> bs(n);
    std::cout << "bits len: " << bs.size() << std::endl;
    std::cout << "bitset: " << bs << std::endl;
    std::cout << toVlqBytes(n) << std::endl;
  }

  /*using Compressor::compress;
  using Decompressor::decompress;
  using Handler::by;

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  CLI::App v2("Farpost access logs compressor/decompressor", "v2");

  v2.add_option("-c,--compress", by(compress), "Compress raw *.log files [filename, ...] to *.v2")
    ->expected(1, INT_MAX);

  v2.add_option("-d,--decompress", by(decompress), "Read compressed *.v2 files [filename, ...] to *.log")
    ->expected(1, INT_MAX);

  CLI11_PARSE(v2, argc, argv)*/
}