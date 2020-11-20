#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include "Compressor.h"
#include "Decompressor.h"
#include "Handler.h"

#include <bit>
#include <bitset>
#include <list>

template<typename Stream, typename Value> concept Output = requires(Stream s, Value v) { s << v; };

template<typename Stream, typename Value> concept Input = requires(Stream s, Value v) { s >> v; };

/**
 * Поток для vlq-компрессии
 */
template<Output<uint64_t> Ostream>
struct VlqOutput {

  explicit constexpr inline VlqOutput(Ostream &o) : output(o) {}

  template<typename Number>
  // TODO C++(final) concept unsigned_integral
  VlqOutput &operator<<(Number n) {
    auto len = std::log2p1(n);
    // auto bits = // берем len bits
    if (len + 1 < free) {
      // то записываем сразу
      output << bitset;
      bitset = {};
    }
  }

private:
  constexpr static ushort length = sizeof(uint64_t) * 8;
  ushort free = length;
  uint64_t bitset{};
  Ostream &output;
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

template<typename N>
inline constexpr bool readBitR(N &&n, uint i) { return (n >> i) & 1u; }


int main(int argc, char **argv) {

  for (std::string buf; std::getline(std::cin, buf);) {
    auto number = std::stoul(buf);
    std::cout << "bitset: " << std::bitset<sizeof(uint) * 8>(number) << std::endl;
    //  TODO: заменить на C++20 <bit> header standards
    auto bit_width = std::log2p1(number);
    for (int i = 0; i < bit_width; ++i) {
      std::cout << readBitR(number, i);
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