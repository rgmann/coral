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

class SerialStream {
public:

  SerialStream(ByteOrder endianness = HostByteOrder);

  void assign(const std::string&);
  void assign(const void* data, ui32 nBytes);

  /**
   * Serialization Methods
   */
  void write(ui8 val);
  void write(ui16 val);
  void write(ui32 val);
  void write(ui64 val);
  //  Appends a null character.
  void writeCString(const std::string& val);
  void write(const std::string& val);
  void write(const char* pData, ui32 nBytes);
  void write(bool val);

  /**
   * Deserialization Methods
   */
  enum ReadStatus {
    ReadOk,
    ReadFail,
    ReadEmpty
  };

  bool read(ui8& val);
  bool read(i8& val);
  bool read(ui16& val);
  bool read(i16& val);
  bool read(ui32& val);
  bool read(i32& val);
  bool read(ui64& val);
  bool read(i64& val);
  bool read(bool& val);

  // Assumes a null termination character.
  ReadStatus readCString(std::string& val);

  // Does not assume a null terminating character.
  ReadStatus read(std::string& val);
  ReadStatus read(char** ppData, ui32& nBytes);
  ReadStatus read(char* pData, ui32 nMaxBytes);

  std::stringstream stream;

private:

  ByteOrder mByteOrder;
};


class Serializable {
public:

  Serializable() {};
  virtual ~Serializable() {};

  std::string serialize() const;
  void serialize(SerialStream& ctor) const;

  std::string serialize();
  void serialize(SerialStream& ctor);

  bool deserialize(const char* pData, ui32 nSizeBytes);
  bool deserialize(const std::string& data);
  bool deserialize(SerialStream& dtor);

protected:

  virtual void pack(SerialStream& rCtor) {};
  virtual void pack(SerialStream& rCtor) const = 0;
  virtual bool unpack(SerialStream& rDtor) = 0;

};

} // End namespace netapp
} // End namespace coral

#endif // SERIALIZABLE_H

