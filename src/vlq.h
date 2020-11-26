#pragma once

#include <ostream>
#include <istream>
#include <cstdint>
#include <istream>

namespace vlq { // заоптимизированные перегрузки для работы с потоками

  using basic_type = uint32_t;

  enum class number : basic_type;

  template<typename T>
  constexpr inline number wrap(T &&n) { return static_cast<vlq::number>(n); }

  template<typename T = basic_type>
  constexpr inline T unwrap(number n) { return static_cast<T>(n); }

  std::ostream &operator<<(std::ostream &, number);

  std::istream &operator>>(std::istream &, number &);
}