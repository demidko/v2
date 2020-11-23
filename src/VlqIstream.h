#pragma once

#include <istream>

struct VlqIstream {

  explicit VlqIstream(std::istream &);

  VlqIstream &operator>>(uint64_t &);

private:
  std::istream &istream;
};

