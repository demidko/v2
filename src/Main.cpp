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

/**
 * Читаем n-ый бит из msb байта
 */
inline constexpr bool readMsb(uint64_t n, uint16_t i) { return (n >> i) & 1u; }


/**
 * Поток для vlq-компрессии
 */
template<typename Output = std::ostream>
struct VlqOutput {

  explicit inline VlqOutput(Output &o) : output(o) {}

  inline ~VlqOutput() { output << bitset; }

  template<typename Value>
  inline VlqOutput &operator<<(Value val) {
    static_assert(
      (sizeof(Value) == (sizeof(void *) * 2)),
      "Values more than sizeof(void*) * 2 bytes must be passed by const reference"
    );
    //  TODO: заменить на C++20 <bit> header standards
    auto valBitSize = std::bit_width(val);
    for (int i = 0, bit; bit = readMsb(val, i), i < valBitSize; ++i) {
      if (!freeBitsetSize) {
        output << bitset;
        bitset = {};
        freeBitsetSize = totalBitsetSize;
      }
      // тут запись в бит
      --freeBitsetSize;
    }
    return this;
  }

private:
  constexpr static uint16_t totalBitsetSize = sizeof(uint64_t) * 8;
  uint16_t freeBitsetSize = totalBitsetSize;
  uint64_t bitset{};
  Output &output;
};


void set_bit(uint64_t &num, uint16_t position) {
  num |= (1u << position);
}

int main(int argc, char **argv) {

  for (std::string buf; std::getline(std::cin, buf);) {

    auto number = std::stoul(buf);
    std::cout << "bitset: " << std::bitset<sizeof(uint16_t) * 8>(number) << std::endl;

    auto bit_width = std::bit_width(number);
    uint64_t res{};

    for (auto i = 0; i < bit_width; ++i) {
      if (readMsb(number, i)) {
        set_bit(res, i);
      }
    }
    std::cout << res << std::endl;
    if (res != number) {
      auto bit_width2 = std::bit_width(res);
      for (int j = 0; j < bit_width2; ++j) {
        std::cout << readMsb(res, j);
      }
      std::cout << std::endl;
    }
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