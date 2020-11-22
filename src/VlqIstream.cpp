#include <VlqIstream.h>
#include <Bit.h>
#include <iostream>

VlqIstream::VlqIstream(std::istream &i) : istream(i) { loadBuffer(); }

VlqIstream &VlqIstream::operator>>(uint64_t &number) {
  number = {};
  uint8_t i = 0;
  for (bool flag = readBit(); flag; flag = readBit()) {
    for (uint8_t j = 0; j < 7; ++j, ++i) {
      auto b = readBit();
      if (b) {
        Bit::set(number, i);
      }
    }
  }
  for (uint8_t j = 0; j < 7; ++j, ++i) {
    auto b = readBit();
    if (b) {
      Bit::set(number, i);
    }
  }
  std::cout << '\n';
  return *this;
}

bool VlqIstream::readBit() {
  if (index == limit) {
    if (istream) {
      loadBuffer();
      return readBit();
    }
    std::cout << 0;
    return 0;
  }
  auto r = Bit::get(buffer, index++);
  std::cout << r;
  return r;
}

void VlqIstream::loadBuffer() {
  buffer = {};
  limit = 8 * istream.readsome(reinterpret_cast<char *>(&buffer), sizeof(buffer));
  index = {};
}



