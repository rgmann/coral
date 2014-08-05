#include <arpa/inet.h>
#include "Log.h"
#include "PacketHelper.h"

using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketCtor::PacketCtor(ByteOrder end)
: mByteOrder(end)
{
}

//-----------------------------------------------------------------------------
void PacketCtor::write(ui8 val)
{
   stream.write((char*)&val, sizeof(ui8));
}

//-----------------------------------------------------------------------------
void PacketCtor::write(ui16 val)
{
   if (mByteOrder == NetworkByteOrder) val = htons(val);
   stream.write((char*)&val, sizeof(ui16));
}

//-----------------------------------------------------------------------------
void PacketCtor::write(ui32 val)
{
   if (mByteOrder == NetworkByteOrder) val = htonl(val);
   stream.write((char*)&val, sizeof(ui32));
}

//-----------------------------------------------------------------------------
void PacketCtor::write(ui64 val)
{
   if (mByteOrder == NetworkByteOrder)
   {
      ui64 upper = (ui64)htonl((ui32)((val >> 32) & 0x00000000FFFFFFFF));
      ui64 lower = (ui64)htonl((ui32)(val & 0x00000000FFFFFFFF));
      val = (lower << 32) | upper;
   }

   stream.write((char*)&val, sizeof(ui64));
}

//-----------------------------------------------------------------------------
void PacketCtor::write(bool val)
{
  stream.write((char*)&val, sizeof(ui8));
}

//-----------------------------------------------------------------------------
void PacketCtor::writeCString(const std::string& val)
{
   PacketCtor::write((ui32)(val.size() + 1));
   stream << val << '\0';
}

//-----------------------------------------------------------------------------
void PacketCtor::write(const std::string& val)
{
   PacketCtor::write((ui32)val.size());
   stream.write(val.data(), val.size());
}

//-----------------------------------------------------------------------------
void PacketCtor::write(const char* pData, ui32 nBytes)
{
  PacketCtor::write(nBytes);
  stream.write(pData, nBytes);
}

//-----------------------------------------------------------------------------
PacketDtor::PacketDtor(ByteOrder end)
: mByteOrder(end)
{
}

//-----------------------------------------------------------------------------
void PacketDtor::setData(const std::string& data)
{
   setData(data.data(), data.size());
}

//-----------------------------------------------------------------------------
void PacketDtor::setData(const char* data, ui32 nSizeBytes)
{
   stream.write(data, nSizeBytes);
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(ui8& val)
{
   stream.read((char*)&val, sizeof(ui8));
   return (stream.fail() == false);
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(i8& val)
{
   ui8 lVal = 0;
   if (!PacketDtor::read(lVal)) return false;
   val = lVal;
   return true;
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(ui16& val)
{
  ui16 lVal = 0;
  stream.read((char*)&lVal, sizeof(ui16));
  if (mByteOrder == NetworkByteOrder) val = ntohs(lVal);
  else val = lVal;
  return (stream.fail() == false);
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(i16& val)
{
   ui16 lVal = 0;
   if (!PacketDtor::read(lVal)) return false;
   val = lVal;
   return true;
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(ui32& val)
{
   ui32 lVal = 0;
   stream.read((char*)&lVal, sizeof(ui32));
   if (mByteOrder == NetworkByteOrder) val = ntohl(lVal);
   else val = lVal;
   return (stream.fail() == false);
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(i32& val)
{
   ui32 lVal = 0;
   if (!PacketDtor::read(lVal)) return false;
   val = lVal;
   return true;
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(ui64& val)
{
   ui64 lVal = 0;
   stream.read((char*)&lVal, sizeof(ui64));
   if (mByteOrder == NetworkByteOrder)
   {
      ui64 upper = (ui64)ntohl((ui32)((lVal >> 32) & 0x00000000FFFFFFFF));
      ui64 lower = (ui64)ntohl((ui32)(lVal & 0x00000000FFFFFFFF));
      val = (lower << 32) | upper;
   }
   else val = lVal;
   return (stream.fail() == false);
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(i64& val)
{
   ui64 lVal = 0;
   if (!PacketDtor::read(lVal)) return false;
   val = lVal;
   return true;
}

//-----------------------------------------------------------------------------
bool PacketDtor::read(bool& val)
{
   stream.read((char*)&val, sizeof(ui8));
   return (stream.fail() == false);
}

//-----------------------------------------------------------------------------
PacketDtor::Status PacketDtor::readCString(std::string& val)
{
   ui32 lnLength = 0;

   if (!PacketDtor::read(lnLength))
   {
     log::error("PacketDtor::readCString - Failed to read string length\n");
     return ReadFail;
   }

   if (lnLength == 0)
   {
     log::error("PacketDtor::readCString - Empty string\n");
     return ReadEmpty;
   }

   char* lpBuffer = new char[lnLength];
   stream.read(lpBuffer, lnLength);
   if (!stream.fail())
   {
      val.assign(lpBuffer);
   }
   delete[] lpBuffer;
   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
PacketDtor::Status PacketDtor::read(std::string& val)
{
   ui32 lnLength = 0;

   if (!PacketDtor::read(lnLength)) return ReadFail;
   if (lnLength == 0) return ReadEmpty;

   char* lpBuffer = new char[lnLength];
   stream.read(lpBuffer, lnLength);
   if (!stream.fail())
   {
      val.assign(lpBuffer, lnLength);
   }
   delete[] lpBuffer;
   //std::cout << "PacketCtor::read(std::string& val): "
   //          << lnLength << std::endl;
   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
PacketDtor::Status PacketDtor::read(char** ppData, ui32& nBytes)
{
   if (!PacketDtor::read(nBytes)) return ReadFail;
   if (nBytes == 0) return ReadEmpty;

   *ppData = new char[nBytes];
   stream.read(*ppData, nBytes);

   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
PacketDtor::Status PacketDtor::read(char* pData, ui32 nMaxBytes)
{
   ui32 lnLength = 0;

   if (PacketDtor::read(lnLength) == false)
   {
     log::error("PacketDtor::read(char* pData, ui32 nBytes) - "
                "Failed to read data length\n");
     return ReadFail;
   }

   if (lnLength == 0)
   {
     log::debug("PacketDtor::read(char* pData, ui32 nBytes) - "
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

   if (stream.fail()) log::error("PacketDtor::read(char* pData, ui32 nBytes) - stream fail read_len=%u, rem=%u, len=%u\n", lnReadLength, lnRemainder, lnLength);

   return stream.fail() ? ReadFail : ReadOk;
}

//-----------------------------------------------------------------------------
std::string Serializable::serialize() const
{
  PacketCtor ctor;
  pack(ctor);
  return ctor.stream.str();
}

//-----------------------------------------------------------------------------
void Serializable::serialize(PacketCtor& ctor) const
{
  pack(ctor);
}

//-----------------------------------------------------------------------------
std::string Serializable::serialize()
{
  PacketCtor ctor;
  pack(ctor);
  return ctor.stream.str();
}

//-----------------------------------------------------------------------------
void Serializable::serialize(PacketCtor& ctor)
{
  pack(ctor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(const char* pData, ui32 nSizeBytes)
{
  PacketDtor dtor;
  dtor.setData(pData, nSizeBytes);
  return unpack(dtor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(const std::string& data)
{
  PacketDtor dtor;
  dtor.setData(data.data(), data.size());
  return unpack(dtor);
}

//-----------------------------------------------------------------------------
bool Serializable::deserialize(PacketDtor& dtor)
{
  return unpack(dtor);
}

