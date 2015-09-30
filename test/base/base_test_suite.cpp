#include <string>
#include "gtest/gtest.h"
#include "arg_parser_test.cpp"
#include "interactive_command_router_test.cpp"
#include "date_test.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

