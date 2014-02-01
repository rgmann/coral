#ifndef RSYNC_SEGMENT_PACKET_H
#define RSYNC_SEGMENT_PACKET_H

#include <string>
#include "GenericPacket.h"
#include "Md5Hash.h"
#include "RsyncCommon.h"

namespace liber {
namespace rsync {

class RsyncSegmentPacket : public liber::netapp::GenericPacket {
public:
   
   enum Type {};
   
   struct __attribute__((__packed__)) Data
   {
      ui32     segmentId;
      i32      weakChecksum;
      Hash128  strongChecksum;
      ui16     segmentSizeBytes;
   };
   
   RsyncSegmentPacket();
   
   ~RsyncSegmentPacket();
   
   bool  setSegmentId(ui32 segmentId);
   bool  getSegmentId(ui32 &segmentId) const;
   
   bool  setWeak(i32 weakChecksum);
   bool  getWeak(i32 &weakChecksum) const;
   
   bool  setStrong(Hash128 strongChecksum);
   bool  getStrong(Hash128 &strongChecksum) const;
   
   bool  setSegmentSize(ui16 segmentSizeBytes);
   bool  getSegmentSize(ui16 &segmentSizeBytes) const;
   
protected:
   
   ui32 dataSize() const;
   
private:
   
   typedef GenericPacket inherited;
   
   Data* segmentData();
   const Data* segmentData() const;
};

}}

#endif // RSYNC_SEGMENT_PACKET_H
