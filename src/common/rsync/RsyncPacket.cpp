#include <stdio.h>
#include "Log.h"
#include "RsyncPacket.h"
#include "ByteOrder.h"

using namespace liber;
using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rsync;



//-----------------------------------------------------------------------------
RsyncTransportPacket::RsyncTransportPacket( int type, const GenericPacket* packet_ptr )
: GenericPacket(sizeof(RsyncTransportPacket::Data), packet_ptr->allocatedSize())
{
  if ( isAllocated() )
  {
    data()->type   = type;
    data()->length = packet_ptr->allocatedSize();

    if ( packet_ptr->allocatedSize() > 0 )
    {
      memcpy(dataPtr(), packet_ptr->basePtr(), packet_ptr->allocatedSize());
    }
  }
}

//-----------------------------------------------------------------------------
void RsyncTransportPacket::swap( void* data_ptr, ui32 length )
{
  RsyncTransportPacket::Data* header_ptr = NULL;

  if ( data_ptr && (length >= sizeof(RsyncTransportPacket::Data)) )
  {
    header_ptr = reinterpret_cast<RsyncTransportPacket::Data*>(data_ptr);

    header_ptr->type   = ByteOrder::NetSwap(header_ptr->type);
    header_ptr->length = ByteOrder::NetSwap(header_ptr->length);
  }
}

//-----------------------------------------------------------------------------
RsyncTransportPacket::Data* const RsyncTransportPacket::data() const
{
  Data* header_ptr = NULL;

  if ( isAllocated() )
  {
    header_ptr = reinterpret_cast<Data*>(basePtr());
  }

  return header_ptr;
}


//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket()
{
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, ui32 length, const void* pData)
: GenericPacket(sizeof(RsyncPacket::Data), length)
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = length;

    if (pData)
    {
      memcpy(dataPtr(), pData, length);
    }
  }
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, const std::string& rData)
: GenericPacket(sizeof(RsyncPacket::Data), rData.size())
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = rData.size();

    if (rData.size() > 0)
    {
      memcpy(dataPtr(), rData.data(), rData.size());
    }
  }
}

//-----------------------------------------------------------------------------
RsyncPacket::Data* const RsyncPacket::data() const
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void RsyncPacket::swap(void* pData, ui32 nSizeBytes)
{
  RsyncPacket::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(RsyncPacket::Data)))
  {
    lpHeader = reinterpret_cast<RsyncPacket::Data*>(pData);

    lpHeader->type   = ByteOrder::NetSwap(lpHeader->type);
    lpHeader->length = ByteOrder::NetSwap(lpHeader->length);
  }
}

//-----------------------------------------------------------------------------
bool RsyncPacket::unpack(const void* pData, ui32 nSizeBytes)
{
   bool lbSuccess = inherited::unpack(pData, nSizeBytes);

   if (lbSuccess)
   {
    lbSuccess = (allocatedSize() >= sizeof(RsyncPacket::Data));

    if (lbSuccess)
    {
      mnDataSizeBytes = sizeof(RsyncPacket::Data);
    }
    else
    {
      log::error("RsyncPacket::unpack - Received buffer is too small\n");
    }
   }

   return lbSuccess;
}

//-----------------------------------------------------------------------------
RsyncPacketLite::RsyncPacketLite( const void* data_ptr, ui32 length )
:  valid_( false )
,  data_ptr_( data_ptr )
,  length_( length )
{
   if ( data_ptr_ && ( length_ >= sizeof(Data) ) )
   {
      valid_ = true;
   }
}

//-----------------------------------------------------------------------------
bool RsyncPacketLite::valid() const
{
   return valid_;
}

//-----------------------------------------------------------------------------
const RsyncPacketLite::Data* RsyncPacketLite::header() const
{
   return reinterpret_cast<const Data*>(data_ptr_);
}

//-----------------------------------------------------------------------------
const void* RsyncPacketLite::data() const
{
   if ( length_ > sizeof(Data) )
   {
      return reinterpret_cast<const ui8*>(data_ptr_) +
             sizeof(Data);
   }
   else
   {
      return NULL;
   }
}

