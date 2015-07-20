#include <string>
#include "gtest/gtest.h"
#include "replay_event_log_test.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

