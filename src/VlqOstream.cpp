#include "VlqOstream.h"
#include <Bit.h>
#include <iostream>

VlqOstream::VlqOstream(std::ostream &o) : ostream(o) {}

VlqOstream::~VlqOstream() { flush(); }

VlqOstream &VlqOstream::operator<<(uint64_t number) {
  uint8_t i = 0;
  auto bitWidth = std::bit_width(number);
  if (bitWidth <= 7) {
    write(0);
    writeOctetFrom(number, i);
    return *this;
  }
  for (; i < bitWidth;) {
    write((bitWidth - i) > 7);
    writeOctetFrom(number, i);
  }
  return *this;
}

void VlqOstream::writeOctetFrom(uint64_t number, uint8_t &i) {
  for (uint8_t limit = i + 7; i < limit; ++i) {
    if (Bit::get(number, i)) {
      write(1);
    }
  }
}

void VlqOstream::write(bool bit) {
  std::cout << bit;
  if (index == 64) {
    flush();
  }
  if (bit) {
    Bit::set(buffer, index);
  }
  ++index;
}

void VlqOstream::flush() {
  ostream.write(reinterpret_cast<char *>(&buffer), sizeof(buffer));
  buffer = {};
  index = {};
}
