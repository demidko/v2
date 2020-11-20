#pragma once

#include <list>
#include <bitset>
#include <ostream>

/**
 * Инкапуслируем работу с битами здесь для любых типов
 */
namespace Vlq {

  template<typename T>
  struct toVlqBits {

    const T value;

    explicit toVlqBits(T &&n) : value(n) {}
  };


  template<typename Number>
  std::ostream &operator<<(std::ostream &out, toVlqBits<Number> &&n) {
    std::bitset<sizeof(Number) * 8> bits(n.value);
    // пропускаем нулевые биты
    auto bitIndex = bits.size();
    for (; bitIndex < bits.size(); ++bitIndex) {
      if (bits[bitIndex] == 1) { break; };
    }
    // двигаемся по значимым битам
    for (; bitIndex < bits.size();) {
      std::byte byte;
      byte << (((bits.size() - bitIndex) > 7) ? 1 : 0);
      auto limit = bitIndex + 7;
      for (; bitIndex < limit && bitIndex < bits.size(); ++bitIndex) {
        byte << bits[bitIndex];
      }
      // выгружаем по одному байту
      out.write(reinterpret_cast<char *>(&byte), 1);
    }
  }

}