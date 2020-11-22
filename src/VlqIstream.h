#pragma once

#include <istream>

struct VlqIstream {

  explicit VlqIstream(std::istream &);

  VlqIstream &operator>>(uint64_t &);

private:

  bool readBit();

  std::istream &istream;
  uint64_t buffer{};
  uint8_t index{};
};

