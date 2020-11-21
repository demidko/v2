#pragma once

#include "Bit.h"
#include <ostream>

/**
 * Поток для vlq-компрессии
 */
struct VlqOstream {

  explicit inline VlqOstream(std::ostream &o) : ostream(o) {}

  inline ~VlqOstream() { ostream << buffer; }

  template<typename N>
  inline void encode(N number) {

    auto len = std::bit_width(number);
    auto octets = std::ceil(len / 7.) - 1;
    uint16_t i = 0;

    for (uint16_t j = 0; j < octets; ++j) {
      write(1);
      for (auto limit = i + 7; i < limit; ++i) { write(Bit::Get(number, i)); }
    }
    write(0);


  }

private:

  inline void write(bool bit) {
    if (index == 64) {
      ostream << buffer;
      buffer = {};
      index = {};
    }
    if (bit) {
      Bit::Set(buffer, index);
    }
    ++index;
  }

  uint16_t index{};
  uint64_t buffer{};
  std::ostream &ostream;
};