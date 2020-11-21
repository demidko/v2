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
    Bit::ForEach(number, [this](auto bit) {
      if (index == 64) {
        ostream << buffer;
        buffer = {};
        index = {};
      }
      if (bit) {
        Bit::Set(buffer, index);
      }
      ++index;
    });
    return this;
  }

private:
  uint16_t index{};
  uint64_t buffer{};
  Ostream &ostream;
};