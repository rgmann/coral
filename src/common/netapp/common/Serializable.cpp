#include <arpa/inet.h>
#include "Log.h"
#include "Serializable.h"

using namespace liber;
using namespace liber::netapp;

void liber::netapp::swapInPlace(i8& val) {}
void liber::netapp::swapInPlace(ui8& val) {}
void liber::netapp::swapInPlace(i16& val)
{
  val = htons(val);
}
void liber::netapp::swapInPlace(ui16& val)
{
  val = htons(val);
}
void liber::netapp::swapInPlace(i32& val)
{
  val = htonl(val);
}
void liber::netapp::swapInPlace(ui32& val)
{
  val = htonl(val);
}
void liber::netapp::swapInPlace(i64& val)
{
  i64 upper = (i64)htonl((ui32)((val >> 32) & 0x00000000FFFFFFFF));
  i64 lower = (i64)htonl((ui32)(val & 0x00000000FFFFFFFF));
  val = (lower << 32) | upper;
}
void liber::netapp::swapInPlace(ui64& val)
{
  ui64 upper = (ui64)htonl((ui32)((val >> 32) & 0x00000000FFFFFFFF));
  ui64 lower = (ui64)htonl((ui32)(val & 0x00000000FFFFFFFF));
  val = (lower << 32) | upper;
}

#define SSTREAM_WRITE(type) \
void SerialStream::write(type val) \
{ \
  if ((mByteOrder == NetworkByteOrder) && (sizeof(type) > 1)) \
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
   if ((mByteOrder == NetworkByteOrder) && (sizeof(type) > 1)) swapInPlace(val); \
   return (stream.fail() == false); \
} \

//-----------------------------------------------------------------------------
SerialStream::SerialStream(ByteOrder end)
: mByteOrder(end)
{
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
void SerialStream::writeCString(const std::string& val)
{
   SerialStream::write((ui32)(val.size() + 1));
   stream << val << '\0';
}

//-----------------------------------------------------------------------------
void SerialStream::write(const std::string& val)
{
   SerialStream::write((ui32)val.size());
   stream.write(val.data(), val.size());
}

//-----------------------------------------------------------------------------
void SerialStream::write(const char* pData, ui32 nBytes)
{
  SerialStream::write(nBytes);
  stream.write(pData, nBytes);
}

//-----------------------------------------------------------------------------
void SerialStream::assign(const std::string& data)
{
   assign(data.data(), data.size());
}

//-----------------------------------------------------------------------------
void SerialStream::assign(const void* data, ui32 nSizeBytes)
{
   stream.write((const char*)data, nSizeBytes);
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
SerialStream::ReadStatus SerialStream::readCString(std::string& val)
{
   ui32 lnLength = 0;

   if (SerialStream::read(lnLength) == false)
   {
     log::error("SerialStream::readCString - Failed to read string length\n");
     return ReadFail;
   }

   if (lnLength == 0)
   {
     log::error("SerialStream::readCString - Empty string\n");
     return ReadEmpty;
   }


   char* lpBuffer = new char[lnLength];
   stream.read(lpBuffer, lnLength);
   if (stream.fail())
   {
     log::error("SerialStream::readCString - stream.fail=true\n");
   }
   else
   {
      val.assign(lpBuffer);
   }

   delete[] lpBuffer;
   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read(std::string& val)
{
   ui32 lnLength = 0;

   if (!SerialStream::read(lnLength)) return ReadFail;
   if (lnLength == 0) return ReadEmpty;

   char* lpBuffer = new char[lnLength];
   stream.read(lpBuffer, lnLength);
   if (!stream.fail())
   {
      val.assign(lpBuffer, lnLength);
   }
   delete[] lpBuffer;
   //std::cout << "SerialStream::read(std::string& val): "
   //          << lnLength << std::endl;
   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read(char** ppData, ui32& nBytes)
{
   if (!SerialStream::read(nBytes)) return ReadFail;
   if (nBytes == 0) return ReadEmpty;

   *ppData = new char[nBytes];
   stream.read(*ppData, nBytes);

   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
SerialStream::ReadStatus SerialStream::read(char* pData, ui32 nMaxBytes)
{
   ui32 lnLength = 0;

   if (SerialStream::read(lnLength) == false)
   {
     log::error("SerialStream::read(char* pData, ui32 nBytes) - "
                "Failed to read data length\n");
     return ReadFail;
   }

   if (lnLength == 0)
   {
     log::debug("SerialStream::read(char* pData, ui32 nBytes) - "
                "Empty data field\n");
     return ReadEmpty;
   }

   ui32 lnReadLength = (nMaxBytes < lnLength) ? nMaxBytes : lnLength;
   ui32 lnRemainder   = (lnReadLength < lnLength) ? (lnLength - lnReadLength) : 0;

   // Read as much as possible into the user supplied buffer.
   stream.read(pData, lnReadLength);

   // Read and throw away whatever is left over.
   if (lnRemainder > 0)
   {
      char* lpBuffer = new char[lnRemainder];
      stream.read(lpBuffer, lnRemainder);
      delete[] lpBuffer;
   }

   if (stream.fail()) log::error("SerialStream::read(char* pData, ui32 nBytes) - stream fail read_len=%u, rem=%u, len=%u\n", lnReadLength, lnRemainder, lnLength);

   return stream.fail() ? ReadFail : ReadOk;
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
std::string Serializable::serialize()
{
  SerialStream ctor;
  pack(ctor);
  return ctor.stream.str();
}

//-----------------------------------------------------------------------------
void Serializable::serialize(SerialStream& ctor)
{
  pack(ctor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(const char* pData, ui32 nSizeBytes)
{
  SerialStream dtor;
  dtor.assign(pData, nSizeBytes);
  return unpack(dtor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(const std::string& data)
{
  SerialStream dtor;
  dtor.assign(data.data(), data.size());
  return unpack(dtor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(SerialStream& dtor)
{
  return unpack(dtor);
}

