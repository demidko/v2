#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <vlq.h>
#include <iostream>
#include <fstream>
#include <bit.h>
#include <filesystem>
#include <list>

TEST_CASE("Bit operations should works correctly") {
  uint64_t expected = 475'187'001;
  uint64_t actual{};
  for (uint8_t i = 0; i < 64; ++i) {
    if (bit::get(expected, i)) {
      bit::set(actual, i);
    }
  }
  REQUIRE(actual == expected);
}

TEST_CASE("Standard operators '>>' and '<<' should works correctly with binary IO") {
  {
    std::ofstream bin_out("test", std::ios::binary);
    bin_out << 47502317ull << "some text and more" << vlq::as_vlq(4) << vlq::as_vlq(475);
  }

  std::ifstream bin_in("test", std::ios::binary);
  uint64_t len;
  vlq::number vlq1, vlq2;
  std::string some, text, and_, more;
  bin_in >> len >> some >> text >> and_ >> more >> vlq1 >> vlq2;
  std::filesystem::remove("test");

  REQUIRE(len == 47502317ull);
  REQUIRE(some == "some");
  REQUIRE(text == "text");
  REQUIRE(and_ == "and");
  REQUIRE(more == "more");
  REQUIRE(vlq::from_vlq<uint64_t>(vlq1) == 4);
  REQUIRE(vlq::from_vlq<uint64_t>(vlq2) == 475);
}

TEST_CASE("Vlq compression should works correctly") {


  constexpr auto test_list_size = 10'000'000;

  auto generate_random_list = [] {
    std::list<uint32_t> list;
    std::generate_n(
      std::back_inserter(list),
      test_list_size,
      std::bind(
        std::uniform_int_distribution<uint64_t>(0, UINT32_MAX),
        std::mt19937{std::random_device{}()}
      )
    );
    return list;
  };

  std::list<uint32_t> expected_list = generate_random_list();
  {
    std::ofstream bin_out("test", std::ios::binary);
    for (auto &&x: expected_list) {
      bin_out << vlq::as_vlq(x);
    }
  }


  std::ifstream bin_in("test", std::ios::binary);
  std::list<uint32_t> actual_list;
  vlq::number buf;
  for (auto i = 0; i < test_list_size; ++i) {
    bin_in >> buf;
    actual_list.push_back(vlq::from_vlq(buf));
  }
  std::filesystem::remove("test");

  REQUIRE(actual_list == expected_list);
}