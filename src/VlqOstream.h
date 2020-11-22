#pragma once

#include <ostream>

struct VlqOstream {

  explicit VlqOstream(std::ostream &);

  virtual ~VlqOstream();

  VlqOstream &operator<<(uint64_t);

private:
  void write(bool);

  void flush();

  uint16_t index{};
  uint64_t buffer{};
  std::ostream &ostream;
};