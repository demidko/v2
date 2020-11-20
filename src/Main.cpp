#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include "Compressor.h"
#include "Decompressor.h"
#include "Handler.h"

#include <bit>
#include <bitset>
#include <list>

/**
 * Читаем n-ый бит из msb последовательности
 */
template<typename N>
inline constexpr bool readMsb(N n, uint i) { return (n >> i) & 1u; }

/**
 * Поток для vlq-компрессии
 */
template<typename Output = std::ostream>
struct VlqOutput {

  explicit inline VlqOutput(Output &o) : output(o) {}

  inline ~VlqOutput() { output << bitset; }

  inline VlqOutput &operator<<(uint64_t n) {
    //  TODO: заменить на C++20 <bit> header standards
    auto nSize = std::log2p1(n);
    for (int i = 0, bit; bit = readMsb(n, i), i < nSize; ++i) {
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

  constexpr static ushort totalBitsetSize = sizeof(uint64_t) * 8;
  ushort freeBitsetSize = totalBitsetSize;
  uint64_t bitset{};
  Output &output;
};


template<typename T>
std::list<std::byte> toVlqBytes(T &&n) {
  std::list<std::byte> result;
  // переводим число в побитовое представление
  std::bitset<sizeof(T) * 8> number(n);
  // пропускаем не заполненные биты
  auto i = number.size();
  for (; i < number.size(); ++i) {
    if (number[i] == 1) { break; };
  }
  // двигаемся по значимым битам
  for (; i < number.size();) {
    // преобразуем
    std::bitset<8> byte;
    byte[0] = (((number.size() - i) > 7) ? 1 : 0);
    for (auto j = 1; j < 8 && i < number.size(); ++j, ++i) {
      byte[j] = number[i];
    }
    // и выгружаем по одному байту
    result.push_back((std::byte) byte.to_ulong());
  }

  return result;
}


int main(int argc, char **argv) {

  for (int i = 0; false, i < 10; ++i) std::cout << i;

  for (std::string buf; std::getline(std::cin, buf);) {
    auto number = std::stoul(buf);
    std::cout << "bitset: " << std::bitset<sizeof(uint) * 8>(number) << std::endl;
    auto bit_width = std::log2p1(number);
    for (int i = 0; i < bit_width; ++i) {
      std::cout << readMsb(number, i);
    }
    std::cout << std::endl;
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