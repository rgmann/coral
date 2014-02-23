#include "EtermServerWorker.h"
#include "RpcAppCommon.h"
#include "HeimdallControllerServerStub.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rpc;

//-----------------------------------------------------------------------------
EtermServerWorker::EtermServerWorker()
{
}

//-----------------------------------------------------------------------------
EtermServerWorker::~EtermServerWorker()
{
}

//-----------------------------------------------------------------------------
bool EtermServerWorker::derivedInitialize()
{
  bool lbSuccess = true;
  mRpcServer.registerResource(new HeimdallControllerServerStub());
  lbSuccess &= router().addSubscriber(RPC_APP_ID, &mRpcServer);
  return lbSuccess;
}

//-----------------------------------------------------------------------------
void EtermServerWorker::derivedDestroy()
{
  router().removeSubscriber(RPC_APP_ID);
}

