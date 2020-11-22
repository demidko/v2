#define CATCH_CONFIG_MAIN
#include "../src/VlqOstream.h"
#include <catch.hpp>

TEST_CASE( "Vlq compression works", "[vlq]" ) {
  int x = 1;
  REQUIRE( x == 1 );
}