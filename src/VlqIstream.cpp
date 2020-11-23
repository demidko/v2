#include <VlqIstream.h>
#include <Bit.h>

VlqIstream::VlqIstream(std::istream &i) : istream(i) {}

VlqIstream &VlqIstream::operator>>(uint64_t &number) {
  for (uint8_t byte, ni{}; ni < 64;) {
    istream.read(reinterpret_cast<char *>(&byte), 1);
    for (uint8_t i = 1; i < 8; ++i, ++ni) {
      if (Bit::get(byte, i)) {
        Bit::set(number, ni);
      }
    }
    if (Bit::get(byte, 0)) {
      break;
    }
  }
  return *this;
}





