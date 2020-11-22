#pragma once

#include "Bit.h"
#include <ostream>

/**
 * Поток для vlq-компрессии
 */
struct VlqOstream {

  explicit VlqOstream(std::ostream &o);

  ~VlqOstream();

  /**
   * Кодирует бит по 7 битным vlq октетам
   * @param number примитив-число не больше 64 бит
   */
  VlqOstream &operator<<(uint64_t number);

private:
  void write(bool bit);

  void flush();

  uint16_t index{};
  uint64_t buffer{};
  std::ostream &ostream;
};