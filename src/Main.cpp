#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include "Compressor.h"
#include "Decompressor.h"
#include "Handler.h"

int main(int argc, char **argv) {

  using Compressor::compress;
  using Decompressor::decompress;
  using Handler::by;

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  CLI::App v2("Farpost access logs compressor/decompressor", "v2");

  v2.add_option("-c,--compress", by(compress), "Compress raw log files [filename, ...]")
    ->expected(1, INT_MAX);

  v2.add_option("-d,--decompress", by(decompress), "Read compressed v2 files [filename, ...]")
    ->expected(1, INT_MAX);

  CLI11_PARSE(v2, argc, argv)
}