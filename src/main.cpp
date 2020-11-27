#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include <nginx_log.h>


template<typename T>
std::function<bool(const std::vector<std::string> &)> by(T &&handler) {
  return [&](auto &files) {
    for (auto &&file: files) { handler(file); };
    return true;
  };
}

int main(int argc, char **argv) {

  std::ios_base::sync_with_stdio(false);

  CLI::App utility("Farpost access logs compressor/decompressor", "v2");

  utility.add_option(
    "-c,--compress", by(nginx_log::compress),
    "Compress raw *.log files [filename, ...] to *.v2"
  )->expected(1, INT32_MAX);

  utility.add_option(
    "-d,--decompress", by(nginx_log::decompress),
    "Read compressed *.v2 files [filename, ...] to *.urls"
  )->expected(1, INT32_MAX);

  CLI11_PARSE(utility, argc, argv)
}