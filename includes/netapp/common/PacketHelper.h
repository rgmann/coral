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



#ifndef PACKET_HELPER_H
#define PACKET_HELPER_H

#include <string>
#include <iostream>
#include <sstream>
#include "BaseTypes.h"

namespace liber {
namespace netapp {

enum ByteOrder {
   HostByteOrder = 0,
   NetworkByteOrder
};

class PacketCtor {
public:

   PacketCtor(ByteOrder end = HostByteOrder);

   void write(ui8 val);
   void write(ui16 val);
   void write(ui32 val);
   void write(ui64 val);
   //  Appends a null character.
   void writeCString(const std::string& val);
   void write(const std::string& val);
   void write(const char* pData, ui32 nBytes);
   void write(bool val);

   std::stringstream stream;

private:

   ByteOrder mByteOrder;
};

class PacketDtor {
public:

   enum Status {
      ReadOk = 0,
      ReadEmpty,
      ReadFail
   };

   PacketDtor(ByteOrder end = HostByteOrder);

   void setData(const std::string& data);
   void setData(const char* data, ui32 nSizeBytes);

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
   Status readCString(std::string& val);

   // Does not assume a null terminating character.
   Status read(std::string& val);
   Status read(char** ppData, ui32& nBytes);
   Status read(char* pData, ui32 nMaxBytes);

   std::stringstream stream;

private:

   ByteOrder mByteOrder;
};


class Serializable {
public:

  Serializable() {};
  virtual ~Serializable() {};

  std::string serialize() const;
  void serialize(PacketCtor& ctor) const;

  std::string serialize();
  void serialize(PacketCtor& ctor);

  bool deserialize(const char* pData, ui32 nSizeBytes);
  bool deserialize(const std::string& data);
  bool deserialize(PacketDtor& dtor);

protected:

  virtual void pack(PacketCtor& rCtor) {};
  virtual void pack(PacketCtor& rCtor) const = 0;
  virtual bool unpack(PacketDtor& rDtor) = 0;

};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_HELPER_H

