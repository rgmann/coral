#include <stdio.h>
#include "AssemblySegmentPacket.h"

//------------------------------------------------------------------------------
AssemblySegmentPacket::AssemblySegmentPacket()
: GenericPacket(sizeof(AssemblySegmentPacket::Data))
{
   if (isAllocated())
   {
      setSegmentId(0);
   }
}

//------------------------------------------------------------------------------
AssemblySegmentPacket::AssemblySegmentPacket(ui32 nSegmentId)
: GenericPacket(sizeof(AssemblySegmentPacket::Data))
{
   if (isAllocated())
   {
      setSegmentId(nSegmentId);
   }
}

//------------------------------------------------------------------------------
void AssemblySegmentPacket::setSegmentId(ui32 nSegmentId)
{
   if (isAllocated())
   {
      data()->segmentId = nSegmentId;
   }
}

//------------------------------------------------------------------------------
bool AssemblySegmentPacket::getSegmentId(ui32 &nSegmentId) const
{
   if (isAllocated())
   {
      nSegmentId = data()->segmentId;
      return true;
   }
   
   return false;
}

//------------------------------------------------------------------------------
AssemblySegmentPacket::Data* const AssemblySegmentPacket::data() const
{
   return reinterpret_cast<Data* const>(dataPtr());
}

//------------------------------------------------------------------------------
ui32 AssemblySegmentPacket::dataSize() const
{
   return sizeof(Data);
}

//------------------------------------------------------------------------------
bool AssemblySegmentPacket::unpack(const void* pPkt, ui32 nSizeBytes)
{   
   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      printf("AssemblySegmentPacket::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the indicated packet size is the same as the
   // actual packet size.
   if (nSizeBytes != dataSize())
   {
      printf("AssemblySegmentPacket::unpack: incorrect size\n");
      return false;
   }
   
   return true;
}
