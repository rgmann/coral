#include <stdio.h>
#include <string.h>
#include <iostream>
#include "RpcPacket.h"

using namespace liber::rpc;
using namespace liber::netapp;

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
   
   if (!lSerialObject.empty() && allocate(sizeof(Data), lSerialObject.size()))
   {
      memcpy(data()->marker, MarkerData, RpcMarkerSize);
      data()->length = lSerialObject.size();
      
      memcpy(dataPtr(), lSerialObject.data(), lSerialObject.size());
   }
}

//------------------------------------------------------------------------------
bool RpcPacket::getObject(RpcObject &object) const
{
   bool lbSuccess = false;
   
   if (isAllocated())
   {
      std::string lSerialObject;
      lSerialObject.assign((char*)dataPtr(), data()->length);

      lbSuccess = object.deserialize(lSerialObject);
      if (!lbSuccess)
      {
         std::cout << "RpcPacket::getObject: Failed to deserialize." << std::endl;
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
      printf("RpcPacket::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the packet is at least large enough to contain the
   // RsyncAssemblyInstr data segment and all preceding data segments.
   if (nSizeBytes < sizeof(Data))
   {
      printf("::unpack: too small\n");
      return false;
   }

   if (strncmp(data()->marker, MarkerData, RpcPacket::RpcMarkerSize) != 0)
   {
      printf("Invalid marker\n");
      return false;
   }
       
   // Validate the size of the packet against the indicated payload size.
   if ((nSizeBytes - sizeof(Data)) != data()->length)
   {
      printf("::unpack: size mismatch - size = %u, exp = %u\n",
             (nSizeBytes - sizeof(Data)), data()->length);
      return false;
   }
       
   return true;
}

//------------------------------------------------------------------------------
/*ui32 RpcPacket::dataSize() const
{
   return sizeof(Data);
}*/

//-----------------------------------------------------------------------------
/*void* RpcPacket::dataPtr()
{
  char* lpData = NULL;
  if (isAllocated()) lpData = (char*)basePtr() + sizeof(Data);
  return (void*)lpData;
}*/

//-----------------------------------------------------------------------------
/*void* const RpcPacket::dataPtr() const
{
  char* lpData = NULL;
  if (isAllocated()) lpData = (char*)basePtr() + sizeof(Data);
  return (void* const)lpData;
}*/

