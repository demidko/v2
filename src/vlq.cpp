#include <vlq.h>
#include <bit.h>

namespace vlq {

  std::ostream &operator<<(std::ostream &out, number n) {
    static char buffer[10];
    auto buffer_byte_index = 0;
    buffer[0] = {};
    auto width = std::bit_width(from_vlq(n));
    for (auto number_bit_index = 0, byte_bit_index = 1; number_bit_index < width; // TODO <= width?
         ++number_bit_index, ++byte_bit_index) {

      if (byte_bit_index == 8) {
        byte_bit_index = 1;
        buffer[++buffer_byte_index] = {};
      }

      if (bit::get(from_vlq(n), number_bit_index)) {
        bit::set(buffer[buffer_byte_index], byte_bit_index);
      }
    }

    // последний байт будет начинаться с 1
    bit::set(buffer[buffer_byte_index], 0);

    out.write(buffer, buffer_byte_index + 1);
    return out;
  }

  std::istream &operator>>(std::istream &in, number &n) {
    n = {};
    auto number_bit_index = 0;
    for (char byte; in.read(&byte, 1);) {
      for (auto i = 1; i < 8; ++i, ++number_bit_index) {
        if (bit::get(byte, i)) {
          bit::set(from_vlq(n), number_bit_index);
        }
      }
      // Если байт начинался с 1 то он был последний
      if (bit::get(byte, 0)) {
        break;
      }
    }
    return in;
  }
}