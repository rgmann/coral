#include "RsyncServerWorker.h"
#include "RpcAppCommon.h"
#include "RsyncServiceServerStub.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rpc;

void ctorHook(RsyncServiceWrapper* pWrapper, void* pData)
{
  RsyncServiceServerWorker* lpWorker = reinterpret_cast<RsyncServiceServerWorker*>(pData);
  lpWorker->router().addSubscriber(RSYNC_SERVICE_ID, 
                     pWrapper->getRsyncPacketSubscriber());
}

//-----------------------------------------------------------------------------
RsyncServiceServerWorker::RsyncServiceServerWorker()
{
}

//-----------------------------------------------------------------------------
RsyncServiceServerWorker::~RsyncServiceServerWorker()
{
}

//-----------------------------------------------------------------------------
bool RsyncServiceServerWorker::derivedInitialize()
{
  bool lbSuccess = true;

  mpRsyncServerStub = new RsyncServiceServerStub();
  mpRsyncServerStub->registerCtorHook(ctorHook, this);

  mRpcServer.registerResource(mpRsyncServerStub);
  lbSuccess &= router().addSubscriber(RPC_APP_ID, &mRpcServer);
  return lbSuccess;
}

//-----------------------------------------------------------------------------
void RsyncServiceServerWorker::derivedDestroy()
{
  router().removeSubscriber(RSYNC_SERVICE_ID);
  router().removeSubscriber(RPC_APP_ID);
}

