#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include <NginxLog.h>
#include <Bit.h>
#include <VlqOstream.h>

/**
 * Функция прокидывает набор файлов по очереди до обработчика
 * @param handler обработчик файла
 * @return обработчик файлов
 */
template<typename Handler>
std::function<bool(const std::vector<std::string> &)> by(Handler &&handler) {
  return [&](auto &files) {
    for (auto &&file: files) { handler(file); };
    return true;
  };
}

int main(int argc, char **argv) {

  CLI::App v2("Farpost access logs compressor/decompressor", "v2");

  v2.add_option(
      "-c,--compress", by(NginxLog::compress),
      "Compress raw *.log files [filename, ...] to *.v2")
    ->expected(1, INT32_MAX);

  v2.add_option(
      "-d,--decompress", by(NginxLog::compress),
      "Read compressed *.v2 files [filename, ...] to *.log")
    ->expected(1, INT32_MAX);

  CLI11_PARSE(v2, argc, argv)
}