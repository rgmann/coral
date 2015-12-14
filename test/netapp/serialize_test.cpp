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

#ifndef  SERIALIZE_TEST_CPP
#define  SERIALIZE_TEST_CPP

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "Log.h"
#include "Serializable.h"

class SerializeTest : public ::testing::Test {
public:

   SerializeTest() {}

protected:

   void SetUp()
   {
      coral::log::level( coral::log::Verbose );
   }

   void TearDown()
   {
      coral::log::flush();
   }
};

TEST_F( SerializeTest, SerializeDeserialize ) {

   using namespace coral::netapp;

   SerialStream stream;

   ui8  ui8_in_val  = 0x12;
   ui16 ui16_in_val = 0x1234;
   ui32 ui32_in_val = 0x12345678;
   ui64 ui64_in_val = 0x0123456789ABCDEF;

   stream.write( ui8_in_val );
   stream.write( ui16_in_val );
   stream.write( ui32_in_val );
   stream.write( ui64_in_val );

   std::string string_in_val("SerializeDeserialize");
   stream.write_string( string_in_val );

   const ui8 DATA_BUFFER[] = {
      0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
   };

   stream.write( DATA_BUFFER, sizeof( DATA_BUFFER ) );
   stream.write( DATA_BUFFER, 6 );


   ui8  ui8_out_val  = 0x12;
   ui16 ui16_out_val = 0x1234;
   ui32 ui32_out_val = 0x12345678;
   ui64 ui64_out_val = 0x0123456789ABCDEF;
   std::string string_out_val;

   EXPECT_EQ( true, stream.read( ui8_out_val ) );
   EXPECT_EQ( ui8_in_val, ui8_out_val );

   EXPECT_EQ( true, stream.read( ui16_out_val ) );
   EXPECT_EQ( ui16_in_val, ui16_out_val );

   EXPECT_EQ( true, stream.read( ui32_out_val ) );
   EXPECT_EQ( ui32_in_val, ui32_out_val );

   EXPECT_EQ( true, stream.read( ui64_out_val ) );
   EXPECT_EQ( ui64_in_val, ui64_out_val );

   EXPECT_EQ( SerialStream::ReadOk, stream.read_string( string_out_val ) );
   EXPECT_STREQ( string_in_val.c_str(), string_out_val.c_str() );


   char* data_ptr = NULL;
   ui32 data_size = 0;

   EXPECT_EQ( SerialStream::ReadOk, stream.read_alloc( &data_ptr, data_size ) );
   EXPECT_EQ( sizeof( DATA_BUFFER ), data_size );
   for (int pos = 0; pos < data_size; ++pos )
   {
      EXPECT_EQ( ui8(DATA_BUFFER[ pos ]), ui8(data_ptr[ pos ]) );
   }
   delete[] data_ptr;


   ui32 read_size = 4;
   data_ptr = new char[ read_size ];
   memset( data_ptr, 0, sizeof( DATA_BUFFER ) );
   EXPECT_EQ( SerialStream::ReadOk, stream.read( data_ptr, read_size ) );
   for (int pos = 0; pos < data_size; ++pos )
   {
      if ( pos < read_size )
         EXPECT_EQ( ui8(DATA_BUFFER[ pos ]), ui8(data_ptr[ pos ]) );
      else
         EXPECT_EQ( ui8(0), ui8(data_ptr[ pos ]) );
   }
   delete[] data_ptr;
}


class TestSerializable : public coral::netapp::Serializable {
public:

   TestSerializable()
      : index(0), buffer(NULL) {};
   TestSerializable( const std::string& n, ui32 ind, ui32 size )
      : index( ind ), buffer( NULL ), buffer_size( 0 ), name( n )
   {
      if ( size > 0 )
      {
         buffer_size = size;
         buffer = new ui8[ buffer_size ];
      }
   }

   ~TestSerializable() {
      if ( buffer ) delete[] buffer;
   }

   ui32 index;
   ui8* buffer;
   ui32 buffer_size;
   std::string name;

protected:
   void pack( SerialStream& ctor ) const
   {
      ctor.write_string(name);
      ctor.write(buffer,buffer_size);
      ctor.write(index);
   }
   bool unpack( SerialStream& dtor )
   {
      bool success = true;
      if ( dtor.read_string( name ) != SerialStream::ReadOk ) {
         success = false;
         log::error("Failed to deserialize 'name' field\n");
      }
      if (buffer) { delete[] buffer; buffer = NULL; }
      if ( success ) {
         if ( dtor.read_alloc( (char**)&buffer, buffer_size ) != SerialStream::ReadOk ) {
            success = false;
            log::error("Failed to deserialize 'buffer' field\n");
         }
      }
      if ( success ) {
         if ( dtor.read( index ) == false ) {
            success = false;
            log::error("Failed to deserialize 'index' field\n");
         }
      }
      return success;
   }
};

TEST_F( SerializeTest, SerializableTest ) {
   using namespace coral::netapp;

   std::string object_name = "TestSerializable";
   ui32 buffer_size = 85;
   TestSerializable source( object_name, 32, buffer_size );


   TestSerializable dest_1;

   std::string obj_str = source.serialize();
   ui32 expected_serial_size = sizeof(ui32) + object_name.size() + 1 +
                               sizeof(ui32) + buffer_size +
                               sizeof(ui32);
   EXPECT_EQ( expected_serial_size, obj_str.size() );

   EXPECT_EQ( true, dest_1.deserialize( obj_str.data(), obj_str.size() ) );
   // EXPECT_EQ( true, dest_1.deserialize( obj_str ) );
   EXPECT_STREQ( source.name.c_str(), dest_1.name.c_str() );
   EXPECT_EQ( source.buffer_size, dest_1.buffer_size );
   EXPECT_EQ( source.index, dest_1.index );


   TestSerializable dest_2;
   dest_2.deserialize( obj_str );
   EXPECT_STREQ( source.name.c_str(), dest_2.name.c_str() );
   EXPECT_EQ( source.buffer_size, dest_2.buffer_size );
   EXPECT_EQ( source.index, dest_2.index );


   TestSerializable dest_3;
   SerialStream dtor;
   source.serialize( dtor );
   dest_3.deserialize( dtor );
   EXPECT_STREQ( source.name.c_str(), dest_3.name.c_str() );
   EXPECT_EQ( source.buffer_size, dest_3.buffer_size );
   EXPECT_EQ( source.index, dest_3.index );
}

#endif // SERIALIZE_TEST_CPP
