#ifndef HEIMDALL_LED_STATE_CMD_H
#define HEIMDALL_LED_STATE_CMD_H

#include "GenericPacket.h"

namespace eterm {

class HeimdallLedStateCommand : public liber::netapp::GenericPacket {
public:

  struct __attribute__((__packed__)) Data {
    ui8  ledId;
    bool ledOn;
  };

  HeimdallLedStateCommand();

  Data* const data();

  void swap(void* pData, ui32 nSizeBytes);
};

}

#endif // HEIMDALL_LED_STATE_CMD_H

