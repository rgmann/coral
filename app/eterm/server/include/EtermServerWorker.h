#ifndef ETERM_SERVER_WORKER_H
#define ETERM_SERVER_WORKER_H

#include "ApplicationWorker.h"
#include "RpcServer.h"

class EtermServerWorker : public liber::netapp::ApplicationWorker {
public:

  EtermServerWorker();
  ~EtermServerWorker();

protected:

  bool derivedInitialize();
  void derivedDestroy();

private:

  liber::rpc::RpcServer mRpcServer;
};

#endif // ETERM_SERVER_WORKER_H

