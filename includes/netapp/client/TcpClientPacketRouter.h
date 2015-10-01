#ifndef TCP_CLIENT_PACKET_ROUTER_H
#define TCP_CLIENT_PACKET_ROUTER_H

#include "TcpSocket.h"
#include "ClientPacketRouter.h"
#include "IThread.h"
// #include "Callback.h"

namespace liber {
namespace netapp {

class TcpSocketReconnectTask : public liber::concurrency::IThread {
public:

  TcpSocketReconnectTask(liber::net::TcpSocket& rSocket, Mutex& rSocketLock);

  void setHost(const char* pHost, int nPort);

private:

  void run(const bool& bShutdown);

private:

  liber::net::TcpSocket& mrSocket;

  Mutex& mrSocketLock;

  const char* mpHost;
  int         mnPort;
};

class DisconnectCallback {
public:
  ~DisconnectCallback(){}
  virtual void call( void* pData ) = 0;
};

class TcpClientPacketRouter : public liber::netapp::ClientPacketRouter {
public:

  TcpClientPacketRouter(bool bAutoReconnect = false);
  virtual ~TcpClientPacketRouter();

  using ClientPacketRouter::start;
  bool start(const char* pHost, int nPort, ui32 txCapacity = DefaultQueueCapacity);

  liber::net::TcpSocket& socket();

  bool setDisconnectCallback(DisconnectCallback* pCallback);

protected:

  bool readPacket(NetAppPacket& rPacket, int nTimeoutMs);
  bool writePacket(const PacketContainer& rPacket); 

private:

  void processDisconnect();

private:

  ui32 mnLockTimeoutMs;

  bool mbAutoReconnect;

  liber::net::TcpSocket mSocket;
  Mutex mSocketLock;

  TcpSocketReconnectTask mReconnectTask;

  DisconnectCallback* mpDisconnectCallback;
  Mutex mCallbackLock;
};

}}

#endif // TCP_CLIENT_PACKET_ROUTER_H

