// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 

#ifndef  HASH_TABLE_TEST_CPP
#define  HASH_TABLE_TEST_CPP

#include <stdio.h>
#include <string.h>
#include "HashTable.h"
#include "Log.h"

class Thingy {
public:

   enum { MaxNameLength = 30 };

   enum ThingyClass {
      BigThingy,
      SmallThingy,
      WetThingy,
      DryThingy,
      SpittyThingy
   };

   enum ThingyLocation {
      OnTheShelf,
      InTheKitchen,
      InTheGarage,
      UnderTheBed,
      InTheCar
   };

   Thingy( int id, const char* name, ThingyClass klass, ThingyLocation location )
    : id_( id ), class_( klass ), location_( location )
   {
      strncpy( name_, name, MaxNameLength );
   }

   int id() const { return id_; };

   char* const name() { return name_; };
   const char* const name() const { return name_; };

   ThingyClass    klass() const { return class_; }
   ThingyLocation location() const { return location_; }

   void print() const
   {
      static const char* CLASS_NAMES[] = {
         "BigThingy",
         "SmallThingy",
         "WetThingy",
         "DryThingy"
      };

      static const char* LOCATION_NAMES[] = {
         "OnTheShelf",
         "InTheKitchen",
         "UnderTheBed",
         "InTheCar"
      };

      coral::log::raw(
         "Thing:\n  id: %d\n name: %s\n  class: %s\n  location: %s\n",
         id_, name_, CLASS_NAMES[ class_ ], LOCATION_NAMES[ location_ ] );
   }

private:

  int id_;
  char name_[ MaxNameLength + 1 ];
  ThingyClass class_;
  ThingyLocation location_;
};

class ThingyComparator {
public:

   ThingyComparator( const char* name ) : test_count_( 0 ), name_( name ) {}

   bool operator() ( const Thingy* thingy_ptr )
   {
      ++test_count_;
      return ( strncmp( thingy_ptr->name(), name_, Thingy::MaxNameLength ) == 0 );
   }

   int test_count_;

private:

   const char* name_;
};


class ThingyDestroyer {
public:
   void operator()( Thingy* pThingy)
   {
      if (pThingy)
      {
         delete pThingy;
         pThingy = NULL;
      }
   }
};


#include "gtest/gtest.h"

using namespace coral;
using namespace coral::cli;

class HashTableTest : public ::testing::Test {
public:

   HashTableTest() {}


protected:

   void SetUp()
   {
      coral::log::level( coral::log::Warn );
   }

   void TearDown()
   {
      coral::log::flush();
   }

};

TEST_F( HashTableTest, AddFindRemoveTest ) {
   Thingy* thing1 = new Thingy(3, "keys", Thingy::SmallThingy, Thingy::UnderTheBed );
   Thingy* thing2 = new Thingy(84, "bicycle", Thingy::BigThingy, Thingy::InTheGarage );
   Thingy* thing3 = new Thingy(48, "car", Thingy::BigThingy, Thingy::InTheGarage );
   Thingy* thing4 = new Thingy(15, "salt", Thingy::DryThingy, Thingy::InTheKitchen );
   Thingy* thing5 = new Thingy(96, "toothbrush", Thingy::WetThingy, Thingy::InTheCar );

   coral::HashTable<16, Thingy*> table;

   table.insert( thing1->klass(), thing1 );
   table.insert( thing2->klass(), thing2 );
   table.insert( thing3->klass(), thing3 );
   table.insert( thing4->klass(), thing4 );
   table.insert( thing5->klass(), thing5 );

   Thingy* temp = NULL;
   ThingyComparator no_item_comparator_1( "llama" );
   EXPECT_EQ( false, table.find( Thingy::SpittyThingy, temp, no_item_comparator_1 ) );
   EXPECT_TRUE( NULL == temp );

   ThingyComparator no_item_comparator_2( "boat" );
   EXPECT_EQ( false, table.find( Thingy::BigThingy, temp, no_item_comparator_2 ) );
   EXPECT_TRUE( NULL == temp );

   ThingyComparator comparator( "car" );
   EXPECT_EQ( true, table.find( Thingy::BigThingy, temp, comparator ) );
   EXPECT_TRUE( NULL != temp );

   // The car was added after the bicycle, so two comparisons should be
   // performed to find the Thingy.
   EXPECT_EQ( 2, comparator.test_count_ );
   if ( temp != NULL )
   {
      EXPECT_EQ( 48, temp->id() );
      EXPECT_EQ( Thingy::InTheGarage, temp->location() );
      temp = NULL;
   }

   // Remove the car from the table.
   EXPECT_EQ( true, table.remove( Thingy::BigThingy, temp, comparator ) );
   EXPECT_TRUE( NULL != temp );
   if ( temp != NULL )
   {
      EXPECT_EQ( 48, temp->id() );
      EXPECT_EQ( Thingy::InTheGarage, temp->location() );
      temp = NULL;
   }

   EXPECT_EQ( false, table.remove( Thingy::BigThingy, temp, comparator ) );
   EXPECT_TRUE( NULL == temp );

   delete thing1;
   delete thing2;
   delete thing3;
   delete thing4;
   delete thing5;
}

#endif  // HASH_TABLE_TEST_CPP
