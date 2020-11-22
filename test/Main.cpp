#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <VlqOstream.h>
#include <VlqIstream.h>
#include <filesystem>
#include <iostream>

TEST_CASE("Vlq compression should works correctly") {

  auto x = 0u;
  REQUIRE(std::bit_width(x) == 0);

  // TODO: генерировать их
  std::vector sourceNumbers{1, 57, 0, 678, 10450, 2, 17, 4567, 2, 10132431, 0, 0, 3, 2221, 3};
  auto filename = "test.bin";

  {
    std::ofstream file(filename);
    VlqOstream ostream(file);

  }

  std::filesystem::remove(filename);
}