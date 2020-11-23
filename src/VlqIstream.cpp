#include <VlqIstream.h>
#include <Bit.h>
#include <iostream>

VlqIstream::VlqIstream(std::istream &i) : istream(i) { loadBuffer(); }

VlqIstream &VlqIstream::operator>>(uint64_t &number) {
  number = {};
  uint8_t i = 0;
  for (bool flag = readBit(); flag; flag = readBit()) {
    readOctetTo(number, i);
  }
  readOctetTo(number, i);
  return *this;
}

void VlqIstream::readOctetTo(uint64_t &number, uint8_t &i) {
  for (uint8_t j = 0; j < 7; ++j, ++i) {
    if (readBit()) {
      Bit::set(number, i);
    }
  }
}

bool VlqIstream::readBit() {
  if (index == limit) {
    if (istream) {
      loadBuffer();
      return readBit();
    }
    return 0;
  }
  return Bit::get(buffer, index++);
}

void VlqIstream::loadBuffer() {
  buffer = {};
  limit = 8 * istream.readsome(reinterpret_cast<char *>(&buffer), sizeof(buffer));
  index = {};
}



