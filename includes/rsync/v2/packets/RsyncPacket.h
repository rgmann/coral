#ifndef  RSYNC_PACKET_H
#define  RSYNC_PACKET_H

#include <stdio.h>
#include "GenericPacket.h"

namespace liber {
namespace rsync {

class RsyncPacket : public liber::netapp::GenericPacket {
public:
   
   enum Type
   {
      TypeNotSet = 0,

      // Ask the server if a file exists
      FileQueryType,

      // Request that the server send the segment report for a file.
      FilePushType,

      // Assembly instruction packet types
      AsmblBeginMarkerType,
      AsmblChunkType,
      AsmblSegmentType,
      AsmblEndMarkerType,

      // Segment report packet types
      SegReportHeaderType,
      SegReportBodyType,

      NumTypes
   };
   
   struct __attribute__((__packed__)) Data
   {
      ui32  marker;
      ui32  type;
      ui32  length;
   };
   static const ui32 RsyncPacketMarker = 0xFEEDBEEF;
   
   RsyncPacket();
   
   RsyncPacket(Type type, ui32 nLengthBytes);
   
   Data* const data() const;
   
   virtual bool unpack(const void* pPkt, ui32 nLengthBytes);

private:
   
   typedef GenericPacket inherited;
};

}}

#endif   // RSYNC_PACKET_H
