#ifndef  SERVER_LISTENER_H
#define  SERVER_LISTENER_H

#include <vector>
#include "BaseTypes.h"
#include "TcpServer.h"
#include "Thread.h"
#include "Queue.h"
#include "ServerNode.h"
#include "ServerWorker.h"

class ServerListener
{
public:
   
   ServerListener();
   
   ~ServerListener();
   
   void  setPort(ui32 port);
   
   bool  start();
   
   bool  stop();
   
protected:
   
   virtual ServerWorker*  createWorker(TcpSocket* pSocket);
   
   bool  distributeWorker(ServerWorker* pWorker);
   
   static void listenThreadFunc(ThreadArg* pArg);
   
   void listenThread(ThreadArg* pArg);
   
   static void distribThreadFunc(ThreadArg* pArg);
   
   void distribThread(ThreadArg* pArg);
   
protected:
   
   ui32        m_nServerPort;
   
   Thread*     m_pListenerThread;
   
   Thread*     m_pBalancerThread;
   
   Queue<ServerWorker*>     m_qWorkerQueue;
   
   ui32        m_nMinNodeCount;
   
   std::vector<ServerNode*>   m_vNodeList;
};

#endif // SERVER_LISTENER_H