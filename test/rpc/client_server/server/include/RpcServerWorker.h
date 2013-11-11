#ifndef  RPC_SERVER_WORKER_H
#define  RPC_SERVER_WORKER_H

#include "ServerWorker.h"
#include "RpcServer.h"

class RpcServerWorker : public ServerWorker
{
public:
   
   RpcServerWorker(TcpSocket* pSocket);
   
   ~RpcServerWorker();
   
   void  setRpcServer(RpcServer *pLog);
   
   bool  processMsg(const char* pMsg, ui32 nMsgLenBytes);
   
   bool  work();
   
   bool  getMsg(char** pMsg, ui32 &nMsgLenBytes);
   
   virtual ui32  headerSize() const;
   
   virtual ui32  getExpPayloadSize(const char* pHeader) const;
   
protected:
   
   RpcServer* mpRpcServer;
};

#endif // RPC_SERVER_WORKER_H
