#define CATCH_CONFIG_MAIN

#include "../src/VlqOstream.h"
#include <catch.hpp>
#include <filesystem>
#include <list>
#include <iostream>

struct VlqBinaryNumber {

  inline operator const uint64_t &() const { return buf; } // NOLINT(google-explicit-constructor)

  VlqBinaryNumber &operator+=(uint8_t octet) {
    for (uint8_t j = 1; j < 8; ++j, ++i) {
      if (Bit::Get(octet, j)) {
        std::cout << 1;
        Bit::Set(buf, i);
      }
      std::cout << 0;
    }
    std::cout << '\n';
    return *this;
  }

private:
  uint64_t buf{};
  uint8_t i = 0;
};

inline uint8_t readOctet(std::istream &vlq) {
  uint8_t octet{};
  vlq.read(reinterpret_cast<char *>(&octet), sizeof(octet));
  return octet;
}

inline std::istream &operator>>(std::istream &vlq, VlqBinaryNumber &n) {
  for (uint8_t octet = readOctet(vlq);
       n += octet, Bit::Get(octet, 0);
       octet = readOctet(vlq)
    );
  return vlq;
}

TEST_CASE("Vlq compression works correctly") {

  std::vector sourceNumbers = {3, 12, 1012, 475, 9, 14, 2000, 10267};

  std::filesystem::path binaryFile("test.bin");

  {
    std::ofstream ostream(binaryFile, std::ios::binary);
    VlqOstream vlq(ostream);
    for (auto &&x: sourceNumbers) {
      vlq << x;
    }
  }

  {
    std::ifstream istream(binaryFile, std::ios::binary);
    for (auto expected : sourceNumbers) {
      VlqBinaryNumber actual;
      istream >> actual;
      // std::cout << actual << std::endl;
      // REQUIRE(actual == expected);
    }
  }

  std::filesystem::remove(binaryFile);
}