#pragma once

#include <ostream>

struct VlqOstream {

  explicit VlqOstream(std::ostream &o);

  virtual ~VlqOstream();

  VlqOstream &operator<<(uint64_t number);

private:
  void write(bool bit);

  void flush();

  uint16_t index{};
  uint64_t buffer{};
  std::ostream &ostream;
};