#ifndef ETERM_SERVER_WORKER_H
#define ETERM_SERVER_WORKER_H

#include "ServerWorker.h"
#include "RpcServer.h"

class EtermServerWorker : public liber::netapp::ServerWorker {
public:

  EtermServerWorker(liber::net::TcpSocket* pSocket);
  ~EtermServerWorker();

protected:

  bool setup();
  void teardown();

private:

  liber::rpc::RpcServer mRpcServer;
};

#endif // ETERM_SERVER_WORKER_H

