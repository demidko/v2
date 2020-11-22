#pragma once

#include <istream>

struct VlqIstream {

  explicit VlqIstream(std::istream &i);

  VlqIstream &operator>>(uint16_t &n);

private:
  std::istream &istream;
  uint64_t buffer{};
  uint8_t index{};
};

