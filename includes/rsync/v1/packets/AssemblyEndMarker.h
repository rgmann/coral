#ifndef ASSEMBLY_END_MARKER_H
#define ASSEMBLY_END_MARKER_H

#include "GenericPacket.h"

class AssemblyEndMarker : public GenericPacket
{
public:
   
   enum Type
   {
   };
   
   struct __attribute__((__packed__)) Data
   {
   };
   
   AssemblyEndMarker();
   
   bool  unpack(void* pPkt, unsigned int nSizeBytes);

private:

   typedef GenericPacket inherited;
};

#endif // ASSEMBLY_END_MARKER_H