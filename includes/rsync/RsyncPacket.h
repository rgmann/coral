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
    RsyncUnknownType,
    RsyncAuthorityService,
    RsyncAuthorityInterface,
    RsyncJobAgent,

    //
    // To RsyncAuthorityService:
    //
    RsyncRemoteAuthQuery,
    RsyncSegment,

    //
    // To RsyncAuthorityInterface:
    //
    RsyncRemoteAuthAcknowledgment,
    RsyncInstruction,
    RsyncAuthorityReport,

    //
    // To RsyncJobAgent:
    //
    RsyncJobRequest,
    RsyncJobComplete
  };

  struct __attribute ((__packed__)) Data
  {
    ui32 length;
    ui32 type;
  };

  RsyncPacket();
  RsyncPacket(int type, ui32 nLength, const void* pData = NULL);
  RsyncPacket(int type, const std::string& rData);
  RsyncPacket(int type, const GenericPacket* pPacket);
   
//  using GenericPacket::allocate; 
//  virtual bool allocate(const Data& rData);

  Data* const data() const;

  void swap(void* pData, ui32 nSizeBytes);

  bool unpack(const void* pData, ui32 nSizeBytes);
   
private:

   typedef liber::netapp::GenericPacket inherited;

};

}}

#endif // RSYNC_PACKET_H
