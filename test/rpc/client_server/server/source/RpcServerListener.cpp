#include "RpcServerWorker.h"
#include "RpcServerListener.h"

//------------------------------------------------------------------------------
RpcServerListener::RpcServerListener() : ServerListener()
{
   mRpcServer.registerResource(&mCalculatorStub);
   mRpcServer.registerResource(&mDatabaseStub);
}

//------------------------------------------------------------------------------
RpcServerListener::~RpcServerListener()
{
}

//------------------------------------------------------------------------------
ServerWorker* RpcServerListener::createWorker(TcpSocket* pSocket)
{
   RpcServerWorker* lpWorker = new RpcServerWorker(pSocket);
   
   if (lpWorker)
   {
      lpWorker->initialize();
      lpWorker->setRpcServer(&mRpcServer);
   }
   
   return lpWorker;
}
