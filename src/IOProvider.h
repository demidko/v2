#pragma once

#include <iostream>
#include <fstream>

namespace IOProvider {

  template<typename Handler>
  std::function<bool(const std::vector<std::string> &)> by(Handler &&handler) {
    return [&handler](auto &&files) {
      if (files.empty()) {
        handler(std::cin);
        return true;
      }
      for (auto &&file: files) {
        std::ifstream stream(file);
        if (!stream) {
          throw std::runtime_error("file '" + file + "' not found");
        }
        handler(stream);
      }
      return true;
    };
  }
}