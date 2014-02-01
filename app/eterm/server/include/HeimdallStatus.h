#ifndef HEIMDALL_STATUS_H
#define HEIMDALL_STATUS_H

#include "GenericPacket.h"

namespace eterm {

class HeimdallStatus : public liber::netapp::GenericPacket {
public:

  struct __attribute__((__packed__)) Data {
    bool success;
    bool ledOn;
    bool fingerprintBeginOk;
    ui16 opCount;
  };

  HeimdallStatus();

  Data* const data();

  void swap(void* pData, ui32 nSizeBytes);
};

}

#endif // HEIMDALL_STATUS_H

