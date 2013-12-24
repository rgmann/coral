#ifndef NET_APP_PACKET_H
#define NET_APP_PACKET_H

#include "GenericPacket.h"

namespace liber {
namespace netapp {

class NetAppPacket : public liber::netapp::GenericPacket {
public:

  struct __attribute__((__packed__)) Data {
    int  type;
    ui32 length;
  };

  NetAppPacket();
  NetAppPacket(int type, ui32 length);
  //virtual ~NetAppPacket();

  using GenericPacket::allocate;
  virtual bool allocate(const Data& rData);

//  virtual void*        dataPtr();
//  virtual void* const  dataPtr() const;
  
  Data* const data();
};

}}

#endif // NET_APP_PACKET_H
