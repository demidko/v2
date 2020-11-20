#pragma once

#include <list>
#include <bitset>
#include <ostream>

/**
 * Инкапуслируем работу с битами здесь для любых типов; Снаружи оперируем байтами
 */
namespace Vlq {

  template<typename T>
  struct toVlqBytes {

    const T value;

    explicit toVlqBytes(const T &n) : value(n) {}
  };

  template<typename Number>
  std::ostream &operator<<(std::ostream &out, toVlqBytes<Number> &&n) {
    // переводим число в побитовое представление
    std::bitset<sizeof(Number) * 8> number(n.value);
    // пропускаем не заполненные биты
    auto i = number.size();
    for (; i < number.size(); ++i) {
      if (number[i] == 1) { break; };
    }
    // двигаемся по значимым битам
    for (; i < number.size();) {
      // преобразуем
      std::bitset<8> byte;
      byte[0] = (((number.size() - i) > 7) ? 1 : 0);
      for (auto j = 1; j < 8 && i < number.size(); ++j, ++i) {
        byte[j] = number[i];
      }
      // и выгружаем по одному байту
      auto symbol = (char) byte.to_ulong();
      out.write(&symbol, 1);
    }
    return out;
  }
}