#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include "Compressor.h"
#include "Decompressor.h"
#include "Handler.h"
#include "Bit.h"
#include <bit>
#include <bitset>
#include <list>
#include "VlqOstream.h"


int main(int argc, char **argv) {

  using Compressor::compress;
  using Decompressor::decompress;
  using Handler::by;

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  CLI::App v2("Farpost access logs compressor/decompressor", "v2");

  v2.add_option("-c,--compress", by(compress), "Compress raw *.log files [filename, ...] to *.v2")
    ->expected(1, INT32_MAX);

  v2.add_option("-d,--decompress", by(decompress), "Read compressed *.v2 files [filename, ...] to *.log")
    ->expected(1, INT32_MAX);

  CLI11_PARSE(v2, argc, argv)
}