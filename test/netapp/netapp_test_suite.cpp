#include <string>
#include "gtest/gtest.h"
#include "packet_router_test.cpp"
#include "packet_relay_test.cpp"
#include "serialize_test.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

