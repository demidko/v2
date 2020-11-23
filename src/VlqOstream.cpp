#include <VlqOstream.h>
#include <Bit.h>
#include <vector>

static std::vector<char> split(uint64_t number) {
  std::vector<char> res;
  uint8_t width = std::bit_width(number);
  uint8_t octets = double(width) / 7;

  for (uint8_t i = 0, ni{}, buf{}; i < octets; buf = {}, ++i) {
    for (uint8_t bi = 1; bi < 8; ++bi, ++ni) {
      if (ni < 64 && Bit::get(number, ni)) Bit::set(buf, bi);
    }
    res.push_back(buf);
  }
  Bit::set(res.back(), 0);
  return res;
}

VlqOstream::VlqOstream(std::ostream &o) : ostream(o) {}


VlqOstream &VlqOstream::operator<<(uint64_t number) {
  for (auto b: split(number)) {
    ostream.write(&b, 1);
  }
  return *this;
}


