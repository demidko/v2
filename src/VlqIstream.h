#pragma once

#include <istream>

struct VlqIstream {

  explicit VlqIstream(std::istream &);

  VlqIstream &operator>>(uint64_t &);

private:

  void readOctetTo(uint64_t &, uint8_t &);

  bool readBit();

  void loadBuffer();

  std::istream &istream;
  uint64_t buffer{};
  uint8_t index{};
  uint8_t limit{};
};

