#include <boost/current_function.hpp>
#include "Log.h"
#include "Date.h"
#include "gtest/gtest.h"

using namespace liber;
using namespace liber::cli;

class DateTimeTest : public ::testing::Test {
public:

   DateTimeTest() {}


protected:

   void SetUp()
   {
      liber::log::level( liber::log::Warn );
   }

   void TearDown()
   {
      liber::log::flush();
   }

};

TEST_F( DateTimeTest, MonthTest ) {
   EXPECT_EQ( "January", Date::Month( 0 ) );
   EXPECT_EQ( "February", Date::Month( 1 ) );
   EXPECT_EQ( "March", Date::Month( 2 ) );
   EXPECT_EQ( "April", Date::Month( 3 ) );   
   EXPECT_EQ( "May", Date::Month( 4 ) );
   EXPECT_EQ( "June", Date::Month( 5 ) );
   EXPECT_EQ( "July", Date::Month( 6 ) );
   EXPECT_EQ( "August", Date::Month( 7 ) );
   EXPECT_EQ( "September", Date::Month( 8 ) );
   EXPECT_EQ( "October", Date::Month( 9 ) );
   EXPECT_EQ( "November", Date::Month( 10 ) );
   EXPECT_EQ( "December", Date::Month( 11 ) );
}

TEST_F( DateTimeTest, DayTest ) {
   EXPECT_EQ( "Sunday", Date::Day( 0 ) );
   EXPECT_EQ( "Monday", Date::Day( 1 ) );
   EXPECT_EQ( "Tuesday", Date::Day( 2 ) );
   EXPECT_EQ( "Wednesday", Date::Day( 3 ) );   
   EXPECT_EQ( "Thursday", Date::Day( 4 ) );
   EXPECT_EQ( "Friday", Date::Day( 5 ) );
   EXPECT_EQ( "Saturday", Date::Day( 6 ) );
}
