#pragma once

#include <string>

namespace Parser {

  /**
   * Переиспользуем для парсинга строк один и тот же поток из статической памяти,
   * чтобы не тратить время на размещение и выделение памяти при конструировании каждый раз.
   * @param text строка для парсинга
   * @return возвращаемый поток легко можно парсить по разделителям при помощи getline(stream, buf, delimeter)
   *
   */
  std::istringstream &parse(const std::string &text);

  /**
   * Функция извлекает слово из текста разделенного пробелами
   */
  std::string extract(const std::string &fromText, int wordAtIndex);
}