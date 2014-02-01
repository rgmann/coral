#ifndef HEIMDALL_CONTROLLER_H
#define HEIMDALL_CONTROLLER_H

#include "BaseTypes.h"
#include "TcpSocket.h"
#include "TcpServer.h"
#include "Mutex.h"
#include "Thread.h"
#include "HeimdallStatus.h"
#include "FingerprintStatus.h"
#include "RpcAppCommon.h"

namespace eterm {

enum HeimdallCommands {
  HdallCommandAck = 1,
  HdallSetLedState = 2,
  HdallFingerprintStatus = 3,
};

class HeimdallController {
public:

  HeimdallController();
  ~HeimdallController();

  struct Status {
    bool connected;
    bool success;
  };

  void setPort(int nPort);

  Status setLedState(LedId id, bool bOn, HeimdallStatus::Data& status);
  Status getFingerprintStatus(HeimdallStatus::Data& hstatus, eterm::FingerprintStatus::Data& fstatus);

private:

  bool connect();
  void disconnect();

  bool sendCommand(i32 commandId, const void* pData, ui32 nBytes); 
  bool recvResponse(void* pData, ui32 nBytes);

private:

  liber::net::TcpSocket* mpSocket;
  Mutex                  mSocketLock;
  int                    mnPort;
};

}

#endif // HEIMDALL_CONTROLLER_H
