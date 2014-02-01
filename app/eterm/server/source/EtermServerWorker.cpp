#include "EtermServerWorker.h"
#include "RpcAppCommon.h"
#include "HeimdallControllerServerStub.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rpc;

//-----------------------------------------------------------------------------
EtermServerWorker::EtermServerWorker(liber::net::TcpSocket* pSocket)
: ServerWorker(pSocket)
{
}

//-----------------------------------------------------------------------------
EtermServerWorker::~EtermServerWorker()
{
}

//-----------------------------------------------------------------------------
bool EtermServerWorker::setup()
{
  bool lbSuccess = true;
  mRpcServer.registerResource(new HeimdallControllerServerStub());
  lbSuccess &= router().addSubscriber(RPC_APP_ID, &mRpcServer);
  return lbSuccess;
}

//-----------------------------------------------------------------------------
void EtermServerWorker::teardown()
{
  router().removeSubscriber(RPC_APP_ID);
}

