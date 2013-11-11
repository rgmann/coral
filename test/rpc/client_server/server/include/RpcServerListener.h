#ifndef  RPC_SERVER_LISTENER_H
#define  RPC_SERVER_LISTENER_H

#include "ServerListener.h"
#include "DatabaseServerStub.h"
#include "CalculatorServerStub.h"
#include "RpcServer.h"

class RpcServerListener : public ServerListener
{
public:
   
   RpcServerListener();
   
   ~RpcServerListener();
   
protected:
   
   virtual ServerWorker*  createWorker(TcpSocket* pSocket);
   
private:
   
   DatabaseServerStub   mDatabaseStub;
   CalculatorServerStub mCalculatorStub;
   RpcServer            mRpcServer;
};

#endif // RPC_SERVER_LISTENER_H
