#include <stdio.h>
#include <string.h>
#include <iostream>
#include "Log.h"
#include "RpcPacket.h"
#include "ByteOrder.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;
using namespace liber::net;

static const char MarkerData[RpcPacket::RpcMarkerSize] = {'r', 'p', 'c',
                                             'm', 's', 'g', '\0'};

//------------------------------------------------------------------------------
RpcPacket::RpcPacket()
: GenericPacket(sizeof(RpcPacket::Data), 0)
{
}

//------------------------------------------------------------------------------
RpcPacket::RpcPacket(const RpcObject &object)
: GenericPacket(sizeof(RpcPacket::Data), 0)
{
   std::string lSerialObject;
   
   lSerialObject = object.serialize();

   if (lSerialObject.empty())
   {
     log::error("RpcPacket - Empty RpcObject. No allocation performed.\n");
   }
   else
   {
     if (allocate(sizeof(Data), lSerialObject.size()))
     {
       memcpy(data()->marker, MarkerData, RpcMarkerSize);
       data()->length = lSerialObject.size();
      
       memcpy(dataPtr(), lSerialObject.data(), lSerialObject.size());
     }
     else
     {
       log::error("RpcPacket - Allocation failed.\n");
     }
   }
}

//------------------------------------------------------------------------------
bool RpcPacket::getObject(RpcObject &object) const
{
   bool lbSuccess = false;
   
   if (isAllocated())
   {
      lbSuccess = object.deserialize((char*)dataPtr(), data()->length);
      if (lbSuccess == false)
      {
         log::error("RpcPacket::getObject: Failed to deserialize.\n");
      }
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
RpcPacket::Data* const RpcPacket::data() const
{
   return reinterpret_cast<Data* const>(basePtr());
}

//------------------------------------------------------------------------------
bool RpcPacket::unpack(const void* pPkt, ui32 nSizeBytes)
{
   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      log::error("RpcPacket::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the packet is at least large enough to contain the
   // RsyncAssemblyInstr data segment and all preceding data segments.
   if (nSizeBytes < sizeof(Data))
   {
      log::error("RpcPacket::unpack: too small\n");
      return false;
   }

   if (strncmp(data()->marker, MarkerData, RpcPacket::RpcMarkerSize) != 0)
   {
      log::error("RpcPacket::unpack: Invalid marker\n");
      return false;
   }
       
   // Validate the size of the packet against the indicated payload size.
   if ((nSizeBytes - sizeof(Data)) != data()->length)
   {
      log::error("RpcPacket::unpack: size mismatch - size = %u, exp = %u\n",
             (nSizeBytes - sizeof(Data)), data()->length);
      return false;
   }
       
   return true;
}

//------------------------------------------------------------------------------
void RpcPacket::swap(void* pData, ui32 nSizeBytes)
{
  if (pData && (nSizeBytes >=  sizeof(RpcPacket::Data)))
  {
    RpcPacket::Data* lpData = reinterpret_cast<RpcPacket::Data*>(pData);

    liber::netapp::swapInPlace(lpData->rpcId);
    liber::netapp::swapInPlace(lpData->length);
    liber::netapp::swapInPlace(lpData->format);
    liber::netapp::swapInPlace(lpData->encoding);
  }
}


