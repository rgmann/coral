#ifndef ETERM_SERVER_WORKER_H
#define ETERM_SERVER_WORKER_H

#include "ApplicationWorker.h"
#include "RpcServer.h"
#include "HeimdallControllerServerStub.h"

class EtermServerWorker : public liber::netapp::ApplicationWorker {
public:

  EtermServerWorker(eterm::HeimdallController& rController);
  ~EtermServerWorker();

  eterm::HeimdallController& getController();

protected:

  bool derivedInitialize();
  void derivedDestroy();

private:

  HeimdallControllerServerStub mHeimdallStub;
  liber::rpc::RpcServer mRpcServer;

  eterm::HeimdallController& mrController;
};

#endif // ETERM_SERVER_WORKER_H

