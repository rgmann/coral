#include <string>
#include "gtest/gtest.h"
#include "end_to_end.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
