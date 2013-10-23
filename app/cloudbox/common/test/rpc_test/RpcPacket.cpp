#include <stdio.h>
#include <string.h>
#include <iostream>
#include "RpcPacket.h"

static const char MarkerData[RpcPacket::RpcMarkerSize] = {'r', 'p', 'c',
                                             'm', 's', 'g'};

//------------------------------------------------------------------------------
RpcPacket::RpcPacket()
: GenericPacket()
{
}

//------------------------------------------------------------------------------
RpcPacket::RpcPacket(const TransportObject &object,
                     TransportObject::Encoding encoding)
: GenericPacket()
{
   ui8* lpData = NULL;
   ui32 lnDataSize = 0;
   
   object.getData(encoding, &lpData, lnDataSize);
      
   if (lpData && allocate(dataSize() + lnDataSize))
   {
      char* lpPayload = reinterpret_cast<char*>(basePtr());
      lpPayload += dataSize();
      
      memcpy(lpPayload, lpData, lnDataSize);
      
      data()->length = lnDataSize;
      data()->encoding = encoding;
      data()->format = object.getFormat();
   }
}

//------------------------------------------------------------------------------
bool RpcPacket::getObject(TransportObject &object) const
{
   bool lbSuccess = false;
   
   if (lbSuccess = isAllocated())
   {
      const ui8* lpPayload = reinterpret_cast<const ui8*>(basePtr());
      lpPayload += dataSize();
      
      object.setData((TransportObject::Encoding)data()->encoding, 
                     lpPayload, data()->length);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
RpcPacket::Data* const RpcPacket::data() const
{
   return reinterpret_cast<Data* const>(dataPtr());
}

//------------------------------------------------------------------------------
bool RpcPacket::unpack(void* pPkt, ui32 nSizeBytes)
{   
   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      printf("RpcPacket::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the packet is at least large enough to contain the
   // RsyncAssemblyInstr data segment and all preceding data segments.
   if (nSizeBytes < dataSize())
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
   if ((nSizeBytes - dataSize()) != data()->length)
   {
      printf("::unpack: size mismatch - size = %u, exp = %u\n",
             (nSizeBytes - dataSize()), data()->length);
      return false;
   }
       
   return true;
}

//------------------------------------------------------------------------------
ui32 RpcPacket::dataSize() const
{
   return sizeof(Data);
}

