#include "VlqOstream.h"
#include <Bit.h>
#include <iostream>

VlqOstream::VlqOstream(std::ostream &o) : ostream(o) {}

VlqOstream::~VlqOstream() { flush(); }

VlqOstream &VlqOstream::operator<<(uint64_t number) {
  if (!number) {
    for (uint8_t i = 0; i < 8; ++i) { write(0); }
    return *this;
  }


  for (uint8_t i = 0, bitWidth = std::bit_width(number); i < bitWidth;) {
    // 1 маркирует полный 7 битный октет
    // 0 маркирует последний, возможно менее чем 7 битный октет
    write((bitWidth - i) > 7);

    for (uint8_t limit = i + 7; i < limit; ++i) {

      auto b = ((i >= bitWidth) ? 0 : Bit::get(number, i));

      // явно заполняем нулями все что выходит за количество кодирующих бит в последнем октете
      // поскольку несмотря на то что там и так в большинстве случаев будут нули
      // все же не исключен случай когда bit_width(n) == sizeof(n) * 8
      // при котором мы рискуем записать после кодирующих бит пару лишних едениц в последний октет
      write((i >= bitWidth) ? 0 : Bit::get(number, i));
    }
  }
  std::cout << '\n';
  return *this;
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