#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <VlqOstream.h>
#include <VlqIstream.h>
#include <filesystem>
#include <iostream>
#include <Bit.h>
#include <bitset>

TEST_CASE("Bit operations should works correctly") {
  uint64_t expected = 475'187'001;
  uint64_t actual{};
  for (uint8_t i = 0; i < 64; ++i) {
    if (Bit::get(expected, i)) {
      Bit::set(actual, i);
    }
  }
  REQUIRE(actual == expected);
}

TEST_CASE("Vlq compression should works correctly") {
  // TODO: генерировать их
  std::vector sourceNumbers{475'187'001};
  auto filename = "test.bin";
  {
    std::ofstream file(filename, std::ios::binary);
    VlqOstream ostream(file);
    for (auto &&x: sourceNumbers) ostream << x;
  }
  /*std::ifstream file(filename, std::ios::binary);
  VlqIstream istream(file);
  uint64_t buf;
  for (auto &&x: sourceNumbers) {
    istream >> buf;
    std::cout << buf << std::endl;
  }
  std::filesystem::remove(filename);*/
}