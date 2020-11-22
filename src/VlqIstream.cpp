#include <VlqIstream.h>
#include <Bit.h>

VlqIstream::VlqIstream(std::istream &i) : istream(i) { loadBuffer(); }

VlqIstream &VlqIstream::operator>>(uint64_t &n) {
  n = {};
  for (uint8_t i = 0, marker = readBit();; marker = readBit()) {
    for (uint8_t j = 0; j < 7; ++j, ++i) {
      if (readBit()) {
        Bit::set(n, i);
      }
    }
    if (!marker) {
      break;
    }
  }
  return *this;
}

bool VlqIstream::readBit() {
  if (index == 64) {
    loadBuffer();
  }
  return Bit::get(buffer, index++);
}

void VlqIstream::loadBuffer() {
  buffer = {};
  istream.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
  index = {};
}



