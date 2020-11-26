#pragma once

#include <string>

namespace nginx_log {

  void compress(const std::string &log_filename);

  void decompress(const std::string &v2_filename);
}
