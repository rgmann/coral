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

}}

#endif // PACKET_HELPER_H

