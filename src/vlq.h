#pragma once

#include <ostream>
#include <istream>
#include <cstdint>
#include <istream>

namespace vlq {
  using basic_type = uint32_t;

  enum class number : basic_type;

  template<typename T>
  constexpr inline number &as_vlq(T &&n) { return reinterpret_cast<vlq::number &>(n); }


  template<typename T = basic_type>
  constexpr inline T &from_vlq(number &n) { return reinterpret_cast<T &>(n); }

  std::ostream &operator<<(std::ostream &, number);

  std::istream &operator>>(std::istream &, number &);
}