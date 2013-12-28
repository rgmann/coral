#include "RpcServerWorker.h"
#include "RpcAppCommon.h"
#include "CalculatorServerStub.h"
#include "DatabaseServerStub.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rpc;

//-----------------------------------------------------------------------------
RpcServerWorker::RpcServerWorker(liber::net::TcpSocket* pSocket)
: ServerWorker(pSocket)
{
}

//-----------------------------------------------------------------------------
RpcServerWorker::~RpcServerWorker()
{
}

//-----------------------------------------------------------------------------
bool RpcServerWorker::setup()
{
  bool lbSuccess = true;
  mRpcServer.registerResource(new CalculatorServerStub());
  mRpcServer.registerResource(new DatabaseServerStub());
  lbSuccess &= router().addSubscriber(RPC_APP_ID, &mRpcServer);
  return lbSuccess;
}

//-----------------------------------------------------------------------------
void RpcServerWorker::teardown()
{
  router().removeSubscriber(RPC_APP_ID);
}

