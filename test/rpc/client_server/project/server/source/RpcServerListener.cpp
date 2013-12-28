#include "RpcServerListener.h"
#include "RpcServerWorker.h"

using namespace liber::netapp;
using namespace liber::net;

//-----------------------------------------------------------------------------
RpcServerListener::RpcServerListener()
: ServerListener()
{
}

//-----------------------------------------------------------------------------
RpcServerListener::~RpcServerListener()
{
}

//-----------------------------------------------------------------------------
ServerWorker* RpcServerListener::createWorker(liber::net::TcpSocket* pSocket)
{
  return new RpcServerWorker(pSocket);
}

