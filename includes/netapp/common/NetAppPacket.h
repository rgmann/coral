#ifndef NET_APP_PACKET_H
#define NET_APP_PACKET_H

#include "GenericPacket.h"

namespace liber {
namespace netapp {

class NetAppPacket : public liber::netapp::GenericPacket {
public:

  struct __attribute__((__packed__)) Data {
    i32  type;
    ui32 length;
  };

  NetAppPacket();
  NetAppPacket(int type, ui32 length);

  using GenericPacket::allocate;
  virtual bool allocate(const Data& rData);

  Data* const data();

  virtual void swap(void* pData, ui32 nSizeBytes);
};

}}

#endif // NET_APP_PACKET_H
