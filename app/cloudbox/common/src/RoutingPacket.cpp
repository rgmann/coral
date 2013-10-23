#include <stdio.h>
#include <string.h>
#include "RoutingPacket.h"

//------------------------------------------------------------------------------
RoutingPacket::RoutingPacket()
: GenericPacket()
{
}

//------------------------------------------------------------------------------
RoutingPacket::RoutingPacket(const GenericPacket* pPacket)
: GenericPacket(nBytes)
{
   from(pPacket);
}

//------------------------------------------------------------------------------
ui32 RoutingPacket::getLength()
{
   ui32 lnSizeBytes = 0;
   
   if (isAllocated())
   {
      lnSizeBytes = data()->length;
   }
   
   return lnSizeBytes;
}

//------------------------------------------------------------------------------
bool RoutingPacket::to(GenericPacket** pPacket)
{   
   if ((*pPacket != NULL) || !isAllocated()) return false;
      
   *pPacket = new GenericPacket(getLength());
   if (*pPacket != NULL) return false;
      
   memcpy((*pPacket)->dataPtr(), dataPtr(), getLength());
   
   return true;
}

//------------------------------------------------------------------------------
bool RoutingPacket::from(const GenericPacket* pPacket)
{
   bool lbSuccess = false;
   
   if (pPacket == NULL) return false;
   
   // If the packet is currently allocated, deallocate it.
   deallocate();
   
   m_nSizeBytes = dataSize() + pPacket->allocatedSize();
   
   if (allocate())
   {
      // Create the header.
      strncpy(data()->marker, Data::marker, sizeof(Data::marker));
      data()->length = nBytes;
      
      // Copy the payload.
      ui8* lpPayload = reinterpret_cast<ui8*>(basePtr());
      
      lpPayload += dataSize();
      
      memcpy(lpPayload, pPacket->dataPtr(), pPacket->allocatedSize());
      
      lbSuccess = true;
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RoutingPacket::unpack(void* pPkt, ui32 nSizeBytes)
{
   if (pPkt == NULL || nSizeBytes < dataSize())
   {
      return false;
   }
   
   // Validate the marker.
   Data* lpData = reinterpret_cast<Data*> (pPkt);
   if (strncmp(lpData->mask, Data::marker, sizeof(Data::marker)) != 0)
   {
      return false;
   }
   
   // Deallocate the packet if it was already allocated.
   deallocate();
   
   // Allocate the packet.
   m_nSizeBytes = nSizeBytes;
   bool lbSuccess = allocate();
   
   // Copy the data.
   if (lbSuccess)
   {
      memcpy(dataPtr(), pPkt, nSizeBytes);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
RoutingPacket::Data* const RoutingPacket::data()
{
   if (!isAllocated()) return NULL;
   
   return reinterpret_cast<Data*> (dataPtr());
}

//------------------------------------------------------------------------------
ui32 RoutingPacket::dataSize() const
{
   return sizeof(Data);
}
