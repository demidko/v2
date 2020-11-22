#pragma once

#include "Bit.h"
#include <ostream>

/**
 * Поток для vlq-компрессии
 */
struct VlqOstream {

  explicit inline VlqOstream(std::ostream &o) : ostream(o) {}

  inline ~VlqOstream() { ostream << buffer; }

  /**
   * Кодирует бит по 7 битным vlq октетам
   * @param number примитив-число не больше 64 бит
   */
  template<typename N>
  inline void encode(N number) {
    // количество минимально необходимых для кодирования числа бит (брать по MSB-first)
    for (uint16_t i = 0, bitWidth = std::bit_width(number); i < bitWidth;) {
      // 1 маркирует полный 7 битный октет
      // 0 маркирует последний, возможно менее чем 7 битный октет
      write((bitWidth - i) > 7);
      for (uint16_t limit = i + 7; i < limit; ++i) {
        // явно заполняем нулями все что выходит за количество кодирующих бит
        // поскольку несмотря на то что там и так в большинстве случаев будут нули
        // все же не исключен случай когда bit_width(n) == sizeof(n) * 8
        // при котором мы рискуем записать после кодирующих бит пару лишних едениц
        // в последний октет
        write((i >= bitWidth) ? 0 : Bit::Get(number, i));
      }
    }
  }

private:

  inline void write(bool bit) {
    if (index == 64) {
      ostream.write(reinterpret_cast<const char *>(&buffer), sizeof(buffer));
      buffer = {};
      index = {};
    }
    if (bit) {
      Bit::Set(buffer, index);
    }
    ++index;
  }

  uint16_t index{};
  uint64_t buffer{};
  std::ostream &ostream;
};