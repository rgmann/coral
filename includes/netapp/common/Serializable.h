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



#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <iostream>
#include <sstream>
#include "BaseTypes.h"

namespace coral {
namespace netapp {

enum ByteOrder {
   HostByteOrder = 0,
   NetworkByteOrder
};

void swapInPlace(i8& val);
void swapInPlace(ui8& val);
void swapInPlace(i16& val);
void swapInPlace(ui16& val);
void swapInPlace(i32& val);
void swapInPlace(ui32& val);
void swapInPlace(i64& val);
void swapInPlace(ui64& val);

///
/// SerialStream is used to serialize or de-serialize an object to or from a
/// stringstream object.
///
class SerialStream {
public:

   ///
   /// Set the serialization/de-serialization byte order
   ///
   /// @param  endianness  Endiannes of stream
   ///
   SerialStream( ByteOrder endianness = HostByteOrder );

   ///
   /// Set the serialization/de-serialization byte order
   ///
   /// @param  data  Buffer to allocate the data from
   /// @param  size_bytes  Size of buffer in bytes
   /// @param  endianness  Endiannes of stream
   ///
   SerialStream( const void* data, ui32 size_bytes, ByteOrder endianness = HostByteOrder );

   ///
   /// Assign contents of string stream. Any current contents are overwritten.
   ///
   /// @param  data  Data string
   /// @return void
   ///
   void assign( const std::string& data );

   ///
   /// Assign contents of string stream. Any current contents are overwritten.
   ///
   /// @param  data  Data string
   /// @return void
   ///
   void assign( const void* data, ui32 size_bytes );

   ///
   /// Serialization Methods
   ///
   void write( ui8 val );
   void write( ui16 val );
   void write( ui32 val );
   void write( ui64 val );
   void write( bool val );

   ///
   /// Serialize a c-style string.
   ///
   /// @param  val  C-style string
   /// @return void
   ///
   void write_string( const std::string& val );

   ///
   /// Serialize a generic block of data. The std::string object is treated
   /// as a convenient byte buffer container rather than a valid c-style string.
   ///
   /// @param  val  Data block
   /// @return void
   ///
   void write( const std::string& val );

   ///
   /// Serialize a generic block of data.
   ///
   /// @param  data  Pointer to data block
   /// @param  size_bytes  Size of block in bytes
   /// @return void
   ///
   void write( const void* data, ui32 size_bytes );


   ///
   /// Deserialization Methods
   ///
   bool read( ui8& val );
   bool read( i8& val );
   bool read( ui16& val );
   bool read( i16& val );
   bool read( ui32& val );
   bool read( i32& val );
   bool read( ui64& val );
   bool read( i64& val );
   bool read( bool& val );

   enum ReadStatus {
      ReadOk,
      ReadFail,
      ReadEmpty
   };

   ///
   /// Deserialize a string field from the stream into a std::string.
   ///
   /// @param  val  Deserialized string field
   /// @return ReadStatus  ReadOk - Buffer was successfully read
   ///                     ReadEmpty - Buffer field was found but size=0
   ///                     ReadFail - Read failed
   ///
   ReadStatus read_string( std::string& val );

   ///
   /// Deserialize a buffer from the stream into a std::string. The string is
   /// used as a convenient container, but the instance should not be treated
   /// as a valid c-style string, but rather as byte array.
   ///
   /// @param  val  Deserialized buffer field
   /// @return ReadStatus  ReadOk - Buffer was successfully read
   ///                     ReadEmpty - Buffer field was found but size=0
   ///                     ReadFail - Read failed
   ///
   ReadStatus read( std::string& val );

   ///
   /// Deserialize a buffer from the stream. At most, max_bytes are read into
   /// the preallocated destination buffer. If the serialized field exists,
   /// and max_bytes is less than the size of the serialize field, the
   /// remaining bytes are read and discarded.
   ///
   /// @param  data  Non-NULL pointer to destination buffer
   /// @param  max_bytes  Maximum number of bytes read
   /// @return ReadStatus  ReadOk - Buffer was successfully read
   ///                     ReadEmpty - Buffer field was found but size=0
   ///                     ReadFail - Read failed
   ///
   ReadStatus read( void* data, ui32 max_bytes );

   ///
   /// Deserialize a buffer from the stream and store it into a dynamically
   /// allocated buffer.
   ///
   /// @param  data  Address of buffer to allocate
   /// @param  size_bytes  Size of allocated buffer
   /// @return ReadStatus  ReadOk - Buffer was successfully read
   ///                     ReadEmpty - Buffer field was found but size=0
   ///                     ReadFail - Read failed
   ///
   ReadStatus read_alloc( char** data, ui32& size_bytes );

   std::stringstream stream;

private:

   // @future
   // Byte order of fields in stream
   ByteOrder byte_order_;
};

///
/// Interface for all serializable classes. A serializable class must
/// implement the "pack" method to serialize an object to a byte stream. It
/// must also implement the "unpack" method to attempt to deserialize an
/// object from a byte stream.
///
class Serializable {
public:

   Serializable() {};
   virtual ~Serializable() {};

   ///
   /// Serialize the object to a string (not a valid c-style string)
   ///
   /// @return std::string  Serialized object
   ///
   // std::string serialize();
   std::string serialize() const;

   ///
   /// Append the serialized object to an existing SerialStream
   ///
   // void serialize( SerialStream& ctor );
   void serialize( SerialStream& ctor ) const;

   bool deserialize( const char* data, ui32 size_bytes );
   bool deserialize( const std::string& data );
   bool deserialize( SerialStream& dtor );

protected:

   virtual void pack(SerialStream& rCtor) {};
   virtual void pack(SerialStream& rCtor) const = 0;
   virtual bool unpack(SerialStream& rDtor) = 0;

};

} // End namespace netapp
} // End namespace coral

#endif // SERIALIZABLE_H

