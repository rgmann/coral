#ifndef FINGERPRINT_STATUS_H
#define FINGERPRINT_STATUS_H

#include "GenericPacket.h"

namespace eterm {

class FingerprintStatus : public liber::netapp::GenericPacket {
public:

  struct __attribute__((__packed__)) Data {
    ui8  numRecords;
    bool fingerPressed;
  };

  FingerprintStatus();

  Data* const data();

  void swap(void* pData, ui32 nSizeBytes);
};

}

#endif // FINGERPRINT_STATUS_H

