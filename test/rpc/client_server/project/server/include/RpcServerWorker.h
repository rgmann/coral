#ifndef RPC_SERVER_WORKER_H
#define RPC_SERVER_WORKER_H

#include "ServerWorker.h"
#include "RpcServer.h"

class RpcServerWorker : public liber::netapp::ServerWorker {
public:

  RpcServerWorker(liber::net::TcpSocket* pSocket);
  ~RpcServerWorker();

protected:

  bool setup();
  void teardown();

private:

  liber::rpc::RpcServer mRpcServer;
};

#endif // RPC_SERVER_WORKER

