#ifndef SERVER_NODE_H
#define SERVER_NODE_H

#include <vector>
#include "Thread.h"
#include "ServerWorker.h"

class ServerNode
{
public:
   
   ServerNode();
   
   ~ServerNode();
   
   bool  addWorker(ServerWorker *pWorker);
   
   int   getWorkerCount();
   
   bool  start();
   
   bool  stop();
   
   float getMeanWorkLat() const;
   
   float getMeanRxLat() const;
   
   float getMeanTxLat() const;
   
private:
   
   static void rxThreadFunc(ThreadArg* pArg);
   
   void        rxThread(ThreadArg* pArg);
   
   static void txThreadFunc(ThreadArg* pArg);
   
   void        txThread(ThreadArg* pArg);
   
   static void workThreadFunc(ThreadArg* pArg);
   
   void        workThread(ThreadArg* pArg);
   
   void        cleanup();
   
private:
   
   //std::vector<ServerWorker*> m_vWorkerList;
   Queue<ServerWorker*> m_TxQueue;
   Queue<ServerWorker*> m_WorkQueue;
   Queue<ServerWorker*> m_RxQueue;
   
   Thread *m_pRxThread;
   
   Thread *m_pWorkerThread;
   
   Thread *m_pTxThread;
};

#endif // SERVER_NODE_H