#include <stdio.h>
#include "AssemblyChunkPacket.h"

//------------------------------------------------------------------------------
AssemblyChunkPacket::AssemblyChunkPacket()
: GenericPacket()
{
}

//------------------------------------------------------------------------------
AssemblyChunkPacket::AssemblyChunkPacket(ui32 nChunkSizeBytes)
 : GenericPacket(nChunkSizeBytes)
{
}

//------------------------------------------------------------------------------
ui32 AssemblyChunkPacket::dataSize() const
{
   return allocatedSize();
}

//------------------------------------------------------------------------------
ui8* const AssemblyChunkPacket::data()
{
   return reinterpret_cast<ui8* const>(basePtr());
}

//------------------------------------------------------------------------------
bool AssemblyChunkPacket::unpack(const void* pPkt, ui32 nSizeBytes)
{   
   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      printf("RsyncAssemblyInstr::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the indicated packet size is the same as the
   // actual packet size.
   if (nSizeBytes != allocatedSize())
   {
      printf("AssemblyChunkPacket::unpack: incorrect size\n");
      return false;
   }
   
   return true;
}
