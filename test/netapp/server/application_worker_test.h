#include <string>
#include "gtest/gtest.h"

namespace {


class ApplicationWorkerTest : public ::testing::Test {
public:

  ApplicationWorkerTest()
  {
  }

  ~ApplicationWorkerTest()
  {
  }
};

TEST_F(ColorTest, ConstructFromString)
{
  Color c("Red");

  EXPECT_EQ(c.getName(), "Red");
  EXPECT_EQ(c.getComponent(Color::Red), 0xFFFFFFFF);
  EXPECT_EQ(c.getComponent(Color::Green), 0);
  EXPECT_EQ(c.getComponent(Color::Blue), 0);
}

TEST_F(ColorTest, SetComponents)
{
  Color c("Grey");
  EXPECT_EQ(c.getName(), "Grey");

  c.setComponent(Color::Red, 255);
  EXPECT_EQ(c.getComponent(Color::Red), 255);

  c.setComponent(Color::Green, 255);
  EXPECT_EQ(c.getComponent(Color::Green), 0);

  c.setComponent(Color::Blue, 255);
  EXPECT_EQ(c.getComponent(Color::Blue), 255);
}

}

