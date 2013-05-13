#ifndef ASSEMBLY_MARKER_PACKET_H
#define ASSEMBLY_MARKER_PACKET_H

#include "RsyncAssemblyInstr.h"

class AssemblyMarkerPacket : public RsyncAssemblyInstr
{
public:
   
   enum Type
   {
      TypeNotSet,
      StartMarkerType,
      EndMarkerType
   };
   
   struct __attribute__((__packed__)) Data
   {
      ui32 type;
   };
   
   AssemblyMarkerPacket();
   
   /**
    * If type = RsyncChunkType, then nParam is treated as the chunk size
    * and used in allocating the data.
    *
    * If type = RsyncSegmentType, then nParam is treated as the segment ID
    * and the data pointer is NULL.
    */
   AssemblyMarkerPacket(Type type);
      
   Type  type() const;
   
   bool  type(Type type);
   
protected:
   
   void* dataPtr();
   
   void* const data() const;

private:

   typedef RsyncAssemblyInstr inherited;
};

#endif // ASSEMBLY_MARKER_PACKET_H