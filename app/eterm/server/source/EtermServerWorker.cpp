#include "EtermServerWorker.h"
#include "RpcAppCommon.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rpc;

void ctorHook(HeimdallControllerWrapper* pWrapper, void* pUserData)
{
  EtermServerWorker* lpServerWorker = reinterpret_cast<EtermServerWorker*>(pUserData);
  pWrapper->setController(&lpServerWorker->getController());
}

//-----------------------------------------------------------------------------
EtermServerWorker::EtermServerWorker(eterm::HeimdallController& rController)
: ApplicationWorker()
, mrController(rController)
{
}

//-----------------------------------------------------------------------------
EtermServerWorker::~EtermServerWorker()
{
}

//-----------------------------------------------------------------------------
eterm::HeimdallController& EtermServerWorker::getController()
{
  return mrController;
}

//-----------------------------------------------------------------------------
bool EtermServerWorker::derivedInitialize()
{
  bool lbSuccess = true;

  mHeimdallStub.registerCtorHook(ctorHook, this);
  mRpcServer.registerResource(&mHeimdallStub);

  lbSuccess &= router().addSubscriber(RPC_APP_ID, &mRpcServer);

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void EtermServerWorker::derivedDestroy()
{
  router().removeSubscriber(RPC_APP_ID);
}

