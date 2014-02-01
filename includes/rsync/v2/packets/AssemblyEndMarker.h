#ifndef ASSEMBLY_END_MARKER_H
#define ASSEMBLY_END_MARKER_H

#include "GenericPacket.h"

namespace liber {
namespace rsync {

class AssemblyEndMarker : public liber::netapp::GenericPacket {
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

   typedef liber::netapp::GenericPacket inherited;
};

}}

#endif // ASSEMBLY_END_MARKER_H
