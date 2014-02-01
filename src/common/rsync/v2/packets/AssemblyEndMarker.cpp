#include "AssemblyEndMarker.h"

//------------------------------------------------------------------------------
AssemblyEndMarker::AssemblyEndMarker()
: GenericPacket(0)
{
}

//------------------------------------------------------------------------------
bool AssemblyEndMarker::unpack(void* pPkt, unsigned int nSizeBytes)
{
   // Empty so there is nothing to do.
   return true;
}