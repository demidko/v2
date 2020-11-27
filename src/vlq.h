#pragma once

#include <ostream>
#include <istream>
#include <cstdint>

/**
 * заоптимизированные перегрузки для работы с потоками
 */
namespace vlq {

  using basic_type = uint32_t;

  /**
   * свой алгебраический тип данных на основе uint32 чтобы добавить перегрузки потокового ввода и вывода,
   * без конфликта с уже существующими операторами для uint32
   */
  enum class number : basic_type;

  template<typename T>
  constexpr inline number wrap(T &&n) { return static_cast<vlq::number>(n); }

  template<typename T = basic_type>
  constexpr inline T unwrap(number n) { return static_cast<T>(n); }

  std::ostream &operator<<(std::ostream &, number);

  std::istream &operator>>(std::istream &, number &);
}