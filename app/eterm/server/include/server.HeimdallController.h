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

  void setPort(int nPort);
  bool isConnected() const;

  bool setLedState(LedId id, bool bOn, HeimdallStatus::Data& status);
  bool getFingerprintStatus(HeimdallStatus::Data& hstatus, eterm::FingerprintStatus::Data& fstatus);

private:

  void destroySocket();

  bool sendCommand(i32 commandId, const void* pData, ui32 nBytes); 
  bool recvResponse(void* pData, ui32 nBytes);

  static void ServerListenThreadEntry(ThreadArg* pArg);
  void serverListenThread(ThreadArg* pArg);

private:

  liber::net::TcpSocket* mpSocket;
  Mutex      mSocketLock;
  liber::net::TcpServer  mTcpServer;
  Thread*    mpServerThread;
  int mnPort;
};

}

#endif // HEIMDALL_CONTROLLER_H
