#pragma once

#include <ostream>

struct VlqOstream {

  explicit VlqOstream(std::ostream &);


  VlqOstream &operator<<(uint64_t);

private:
  std::ostream &ostream;
};