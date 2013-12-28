#ifndef RPC_SERVER_LISTENER_H
#define RPC_SERVER_LISTENER_H

#include "ServerListener.h"

class RpcServerListener : public liber::netapp::ServerListener {
public:

  RpcServerListener();
  ~RpcServerListener();

protected:

  liber::netapp::ServerWorker* createWorker(liber::net::TcpSocket* pSocket);

};

#endif // RPC_SERVER_LISTENER_H

