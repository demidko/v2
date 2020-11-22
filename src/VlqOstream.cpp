#include "VlqOstream.h"

VlqOstream::VlqOstream(std::ostream &o) : ostream(o) {}

VlqOstream::~VlqOstream() { flush(); }

VlqOstream &VlqOstream::operator<<(uint64_t number) {
  for (uint16_t i = 0, bitWidth = std::bit_width(number); i < bitWidth;) {
    // 1 маркирует полный 7 битный октет
    // 0 маркирует последний, возможно менее чем 7 битный октет
    write((bitWidth - i) > 7);
    for (uint16_t limit = i + 7; i < limit; ++i) {
      // явно заполняем нулями все что выходит за количество кодирующих бит в последнем октете
      // поскольку несмотря на то что там и так в большинстве случаев будут нули
      // все же не исключен случай когда bit_width(n) == sizeof(n) * 8
      // при котором мы рискуем записать после кодирующих бит пару лишних едениц
      // в последний октет
      write((i >= bitWidth) ? 0 : Bit::Get(number, i));
    }
  }
  return *this;
}

void VlqOstream::write(bool bit) {
  if (index == 64) {
    flush();
  }
  if (bit) {
    Bit::Set(buffer, index);
  }
  ++index;
}

void VlqOstream::flush() {
  ostream.write(reinterpret_cast<char *>(&buffer), sizeof(buffer));
  buffer = {};
  index = {};
}

