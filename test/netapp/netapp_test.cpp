#include <string>
#include "gtest/gtest.h"
#include "color_test.h"
/*namespace {

class Color {
public:

  Color(const std::string& name)
  {
    for (int index = 0; index < 3; index++) mComponents[index] = 0;
    if (name == "Red") mComponents[Red] = 0xFFFFFFFF;
    if (name == "Green") mComponents[Green] = 0xFFFFFFFF;
    if (name == "Blue") mComponents[Blue] = 0xFFFFFFFF;
    mName = name;
  };

  std::string& getName()
  {
    return mName;
  };

  enum Component {
    Red,
    Green,
    Blue
  };
  void setComponents(Component component, int value)
  {
    mComponents[component] = value;
  };

  int getComponent(Component component)
  {
    return mComponents[component];
  };

private:

  std::string mName;
  int mComponents[3];
};

class ColorTest : public ::testing::Test {
public:

  ColorTest()
  {
  }

  ~ColorTest()
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
}
*/
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

