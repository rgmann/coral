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



#include <arpa/inet.h>
#include "Log.h"
#include "Serializable.h"

using namespace coral;
using namespace coral::netapp;

void coral::netapp::swapInPlace(i8& val) {}
void coral::netapp::swapInPlace(ui8& val) {}
void coral::netapp::swapInPlace(i16& val)
{
  val = htons(val);
}
void coral::netapp::swapInPlace(ui16& val)
{
  val = htons(val);
}
void coral::netapp::swapInPlace(i32& val)
{
  val = htonl(val);
}
void coral::netapp::swapInPlace(ui32& val)
{
  val = htonl(val);
}
void coral::netapp::swapInPlace(i64& val)
{
  i64 upper = (i64)htonl((ui32)((val >> 32) & 0x00000000FFFFFFFF));
  i64 lower = (i64)htonl((ui32)(val & 0x00000000FFFFFFFF));
  val = (lower << 32) | upper;
}
void coral::netapp::swapInPlace(ui64& val)
{
  ui64 upper = (ui64)htonl((ui32)((val >> 32) & 0x00000000FFFFFFFF));
  ui64 lower = (ui64)htonl((ui32)(val & 0x00000000FFFFFFFF));
  val = (lower << 32) | upper;
}

#define SSTREAM_WRITE(type) \
void SerialStream::write(type val) \
{ \
  if ((byte_order_ == NetworkByteOrder) && (sizeof(type) > 1)) \
  { \
    swapInPlace(val); \
  } \
  stream.write((char*)&val, sizeof(type)); \
} \

#define SSTREAM_IREAD(type, utype) \
bool SerialStream::read(type& val) \
{ \
   utype lVal = 0; \
   if (!SerialStream::read(lVal)) return false; \
   val = lVal; \
   return true; \
} \


#define SSTREAM_UREAD(type) \
bool SerialStream::read(type& val) \
{ \
   stream.read((char*)&val, sizeof(type)); \
   if ((byte_order_ == NetworkByteOrder) && (sizeof(type) > 1)) swapInPlace(val); \
   return (stream.fail() == false); \
} \

//-----------------------------------------------------------------------------
SerialStream::SerialStream( ByteOrder byte_order )
   : byte_order_( byte_order )
{
}

//-----------------------------------------------------------------------------
SerialStream::SerialStream(
   const void* data,
   ui32        size_bytes,
   ByteOrder   byte_order
)
   : byte_order_( byte_order )
{
   assign( data, size_bytes );
}

//-----------------------------------------------------------------------------
SSTREAM_WRITE(ui8)
SSTREAM_WRITE(ui16)
SSTREAM_WRITE(ui32)
SSTREAM_WRITE(ui64)

//-----------------------------------------------------------------------------
void SerialStream::write(bool val)
{
  stream.write((char*)&val, sizeof(ui8));
}

//-----------------------------------------------------------------------------
void SerialStream::write_string(const std::string& val)
{
   SerialStream::write((ui32)(val.size() + 1));
   stream << val << '\0';
}

//-----------------------------------------------------------------------------
void SerialStream::write( const std::string& val)
{
   SerialStream::write((ui32)val.size() );
   stream.write(val.data(), val.size());
}

//-----------------------------------------------------------------------------
void SerialStream::write( const void* data_ptr, ui32 size_bytes )
{
   SerialStream::write( size_bytes );
   stream.write( (const char*)data_ptr, size_bytes );
}

//-----------------------------------------------------------------------------
void SerialStream::assign(const std::string& data)
{
   assign(data.data(), data.size());
}

//-----------------------------------------------------------------------------
void SerialStream::assign( const void* data, ui32 size_bytes )
{
   stream.write( (const char*)data, size_bytes );
   if ( stream.bad() || stream.fail() ) log::error("SerialStream::assign failed\n");
}

//-----------------------------------------------------------------------------
SSTREAM_IREAD(i8, ui8)
SSTREAM_UREAD(ui8)
SSTREAM_IREAD(i16, ui16)
SSTREAM_UREAD(ui16)
SSTREAM_IREAD(i32, ui32)
SSTREAM_UREAD(ui32)
SSTREAM_IREAD(i64, ui64)
SSTREAM_UREAD(ui64)

//-----------------------------------------------------------------------------
bool SerialStream::read(bool& val)
{
   stream.read((char*)&val, sizeof(ui8));
   return (stream.fail() == false);
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read_string( std::string& val )
{
   ReadStatus status = ReadFail;
   ui32 string_length = 0;

   if ( SerialStream::read( string_length ) )
   {
      if ( string_length > 0 )
      {
         char* string_buffer = new char[ string_length ];

         stream.read( string_buffer, string_length );

         if ( stream.fail() )
         {
            log::error("SerialStream::read_string - stream.fail=true\n");
            status = ReadFail;
         }
         else
         {
            // Make sure the string if NULL terminated.
            string_buffer[ string_length - 1 ] = 0;

            val.assign( string_buffer );
            status = ReadOk;
         }

         delete[] string_buffer;
      }
      else
      {
         log::error("SerialStream::read_string - Empty string\n");
         status = ReadEmpty;
      }
   }
   else
   {
      log::error("SerialStream::read_string - Failed to read string length\n");
      status = ReadFail;
   }

   return status;
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read( std::string& val )
{
   ReadStatus status = ReadFail;
   ui32 data_length = 0;

   if ( SerialStream::read( data_length ) )
   {
      if ( data_length > 0 )
      {
         char* data_buffer = new char[ data_length ];

         stream.read( data_buffer, data_length );

         if ( stream.fail() )
         {
            status = ReadFail;
         }
         else
         {
            val.assign( data_buffer, data_length );
            status = ReadOk;
         }

         delete[] data_buffer;
      }
      else
      {
         log::error("SerialStream::read - Empty string\n");
         status = ReadEmpty;
      }
   }
   else
   {
      log::error("SerialStream::read - Failed to read data length\n");
      status = ReadFail;
   }

   return status;
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read_alloc( char** data, ui32& size_bytes )
{
   ReadStatus status = ReadFail;

   if ( ( *data == NULL ) && SerialStream::read( size_bytes ) )
   {
      if ( size_bytes > 0 )
      {
         *data = new char[ size_bytes ];

         stream.read( *data, size_bytes );

         if ( stream.fail() )
         {
            log::error("SerialStream::read: Failed to buffer block from stream.\n");

            size_bytes = 0;
            delete[] *data;
            *data = NULL;
         }
         else
         {
            status = ReadOk;
         }
      }
      else
      {
         log::error("SerialStream::read: Empty block.\n");
         status = ReadEmpty;
      }
   }
   else
   {
      log::error("SerialStream::read: Failed to read data length\n");
      status = ReadFail;
   }

   return status;
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read( void* data, ui32 max_bytes )
{
   ReadStatus status = ReadFail;
   ui32 size_bytes = 0;

   if ( data )
   {
      if ( SerialStream::read( size_bytes ) )
      {
         if ( size_bytes )
         {
            ui32 bytes_to_read = size_bytes;

            if ( bytes_to_read > max_bytes )
            {
               bytes_to_read = max_bytes;
            }

            ui32 bytes_remaining = size_bytes - bytes_to_read;

            stream.read( reinterpret_cast<char*>( data ), bytes_to_read );

            if ( stream.fail() )
            {
               log::error("SerialStream::read: Stream read failure.\n");
               status = ReadFail;
            }
            else
            {
               if ( bytes_remaining > 0 )
               {
                  char* remainder_buffer = new char[ bytes_remaining ];
                  stream.read( remainder_buffer, bytes_remaining );
                  delete[] remainder_buffer;
               }

               status = ReadOk;
            }
         }
         else
         {
            status = ReadEmpty;
         }
      }
      else
      {
         log::error("SerialStream::read(char* pData, ui32 nBytes) - "
                   "Failed to read data length\n");
         status = ReadFail;
      }
   }
   else
   {
      status = ReadFail;
   }

   return status;
}

//-----------------------------------------------------------------------------
std::string Serializable::serialize() const
{
  SerialStream ctor;
  pack(ctor);
  return ctor.stream.str();
}

//-----------------------------------------------------------------------------
void Serializable::serialize(SerialStream& ctor) const
{
  pack(ctor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize( const char* data, ui32 size_bytes )
{
  SerialStream dtor( data, size_bytes );
  // dtor.assign( data, size_bytes );
  return unpack(dtor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(const std::string& data)
{
  SerialStream dtor( data.data(), data.size() );
  // dtor.assign(data.data(), data.size());
  return unpack( dtor );
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(SerialStream& dtor)
{
  return unpack(dtor);
}

