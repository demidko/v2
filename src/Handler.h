#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>

namespace Handler {

  /**
   * Функция прокидывает набор файлов по очереди до обработчика
   * @param handler обработчик файла
   * @return обработчик файлов
   */
  template<typename Handler>
  std::function<bool(const std::vector<std::string> &)> by(Handler &&handler) {
    return [&](auto &files) {
      for (auto &&file: files) handler(file);
      return true;
    };
  }
}