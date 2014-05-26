#ifndef RSYNC_PACKET_H
#define RSYNC_PACKET_H

#include <string>
#include "GenericPacket.h"

namespace liber {
namespace rsync {

class RsyncPacket : public liber::netapp::GenericPacket {
public:

  enum Type
  {
    RsyncRemoteJobQuery,
    RsyncRemoteJobAcknowledgment,

    RsyncSegment,

    RsyncInstruction
  };

  enum { RsyncMarkerSize = 7 };
  struct __attribute ((__packed__)) Data
  {
    char marker[RsyncMarkerSize];
    ui32 length;
    ui32 type;
  };

  RsyncPacket();
  RsyncPacket(int type, ui32 nLength, const void* pData = NULL);
  RsyncPacket(int type, const std::string& rData);
   
  using GenericPacket::allocate; 
  virtual bool allocate(const Data& rData);

  Data* const data() const;

  void swap(void* pData, ui32 nSizeBytes);
   
private:

   typedef liber::netapp::GenericPacket inherited;

};

}}

#endif // RSYNC_PACKET_H
