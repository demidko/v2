#pragma once

#include "Bit.h"

/**
 * Поток для vlq-компрессии
 */
template<typename Ostream>
struct VlqOstream {

  explicit inline VlqOstream(Ostream &o) : ostream(o) {}

  inline ~VlqOstream() { ostream << buffer; }

  template<typename N>
  inline VlqOstream &operator<<(N number) {
    Bit::ForEach(number, [this](bool bit) {
      if (index == limit) {
        flush();
      }
      if (bit) {
        Bit::Set(buffer, index);
      }
      ++index;
    });
    return this;
  }

private:

  inline void flush() {
    ostream << buffer;
    buffer = {};
    index = {};
  }

  constexpr static uint16_t limit{64};
  uint16_t index{};
  uint64_t buffer{};

  Ostream &ostream;
};