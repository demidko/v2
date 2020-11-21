#pragma once
/**
 * Самописная библиотека для работы с битами. Используется MSB-fisrt порядок.
 */
#include <bit>
/**
 * Для работы нам нужны низкоуровневые побитовые операции стандарта C++20,
 * определенные в файле <bit>: https://en.cppreference.com/w/cpp/header/bit
 *
 * Ранее эти функции присутствовали в предварительном стандарте C++2a, под другими именами,
 * но комитет по стандартизации решил переименовать их ради большей выразительности, а старые имена удалить.
 * Тут-то и началось все веселье.
 *
 * В библиотеке libstdc++, обычно используемой GCC, файл <bit> соответствует C++20 стадарту,
 * и предварительные имена из C++2a не поддерживаются.
 *
 * Однако в библиотеке libc++ из LLVM, обычно используемой clang, файл <bit> до сих пор соответствует C++2a,
 * и не приведен в соответствие С++20, поэтому в нем наборот, не поддерживаются новые имена.
 * Обсуждение на сайте LLVM Project: https://reviews.llvm.org/D90551
 * Судя по всему LLVM Project будет и дальше поддерживать старые имена ради совместимости.
 *
 * Поэтому, чтобы проект собирался и с libstc++ и с libc++,
 * приводим на этапе компиляции имена из libc++ в соответствие стандарту,
 * определяя для них подстановку внутри стандартного пространства имён.
 */
#ifdef __llvm__
namespace std {

#define bit_width log2p1
#define bit_floor floor2
#define bit_ceil ceil2
#define has_single_bit ispow2

}
#endif

namespace Bit {
  /**
   * Читаем i-ый бит из n. Используется MSB-first порядок.
   */
  template<typename N>
  inline constexpr bool Get(N n, uint16_t i) { return (n >> i) & 1u; }

  /**
   * Записываем i-ый бит в n. Используется MSB-first порядок.
   */
  template<typename N>
  inline constexpr void Set(N &n, uint16_t i) { n |= (1u << i); }

  /**
   * Обходим все биты в MSB-first порядке.
   */
  template<typename N, typename F>
  void ForEach(N number, F &&func) {
    for (uint16_t i = 0, len = std::bit_width(number), bit; bit = Get(number, i), i < len; ++i) {
      func(bit);
    }
  }
}