#include <stdio.h>
#include "ServerListener.h"
#include "TcpServer.h"

//------------------------------------------------------------------------------
ServerListener::ServerListener()
{
   m_pListenerThread = NULL;
   m_pBalancerThread = NULL;
   m_nMinNodeCount = 1;
}

//------------------------------------------------------------------------------
ServerListener::~ServerListener()
{
   if (m_pListenerThread)
   {
      delete m_pListenerThread;
      m_pListenerThread = NULL;
   }
   
   if (m_pBalancerThread)
   {
      delete m_pBalancerThread;
      m_pBalancerThread = NULL;
   }
}

//------------------------------------------------------------------------------
void  ServerListener::setPort(ui32 port)
{
   m_nServerPort = port;
}

//------------------------------------------------------------------------------
bool  ServerListener::start()
{
   m_pListenerThread = Thread::Create(listenThreadFunc, this);
   
   if (m_pListenerThread == NULL)
   {
      printf("ServerListener::start: Failed to create listener thread.\n");
      return false;
   }
   
   m_pBalancerThread = Thread::Create(distribThreadFunc, this);
   
   if (m_pBalancerThread == NULL)
   {
      printf("ServerListener::start: Failed to create listener thread.\n");
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool  ServerListener::stop()
{
   if (m_pListenerThread)
   {
      m_pListenerThread->stop();
      m_pListenerThread->join();
   }
   
   if (m_pBalancerThread)
   {
      m_pBalancerThread->stop();
      m_pBalancerThread->join();
   }
   
   printf("ServerListener::stop: Stopped server.\n");
   
   return true;
}

//------------------------------------------------------------------------------
void  ServerListener::listenThreadFunc(ThreadArg* pArg)
{
   ((ServerListener*)pArg->pUserData)->listenThread(pArg);
}

//------------------------------------------------------------------------------
void  ServerListener::listenThread(ThreadArg* pArg)
{
   TcpServer   l_TcpServer;
   TcpSocket*  l_pSocket = NULL;
   
   ServerWorker*  l_pWorker = NULL;
   
   // Start the server on the specified port
   printf("ServerListener: Starting server on port #%d\n", m_nServerPort);
   l_TcpServer.start(m_nServerPort);
   
   // Main thread loop
   while (!pArg->stopSignalled())
   {
      l_pSocket = l_TcpServer.acceptClient(500);
      
      if (l_pSocket == NULL)
      {
         continue;
      }
      
      // The inheriting class overrides the createWorker method to create
      // the appropiriate type of worker.
      printf("ServerListener: Received connection. Creating worker.\n");
      l_pWorker = createWorker(l_pSocket);
      
      // If the worker was successfully created pass the worker to the
      // distribution and load balancing thread.
      if (l_pWorker)
      {
         distributeWorker(l_pWorker);
      }
      
      // We're now done with the pointers, so reset them to NULL.
      l_pWorker = NULL;
      l_pSocket = NULL;
   }
   
   // Stop the TCP server
   l_TcpServer.stop();
}

//------------------------------------------------------------------------------
ServerWorker*  ServerListener::createWorker(TcpSocket* pSocket)
{
   return NULL;
}

//------------------------------------------------------------------------------
bool ServerListener::distributeWorker(ServerWorker* pWorker)
{
   return m_qWorkerQueue.push(pWorker, 500);
}

//------------------------------------------------------------------------------
void  ServerListener::distribThreadFunc(ThreadArg* pArg)
{
   ((ServerListener*)pArg->pUserData)->distribThread(pArg);
}

//------------------------------------------------------------------------------
void  ServerListener::distribThread(ThreadArg* pArg)
{
   ui32  l_nCurrentNode = 0;
   ServerWorker*  l_pWorker = NULL;
   
   // Add and start the minimum number of nodes
   while (m_vNodeList.size() < m_nMinNodeCount)
   {
      ServerNode* l_pNode = NULL;
      
      l_pNode = new ServerNode();
      if (l_pNode == NULL)
      {
         printf("ServerListener::listenThread: Failed to allocate node\n");
         break;
      }
      
      // Start the node
      l_pNode->start();
      
      // Add the node to the node list
      m_vNodeList.push_back(l_pNode);
   }
   
   // Main thread loop
   while (!pArg->stopSignalled())
   {
      // Wait briefly for a new worker
      m_qWorkerQueue.pop(l_pWorker, 500);
      
      // If the worker is not NULL then we can add it to a server node.
      // Currently, round-robin is the only assignment policy.
      if (m_vNodeList[l_nCurrentNode] != NULL)
      {
         m_vNodeList[l_nCurrentNode]->addWorker(l_pWorker);
         
         l_nCurrentNode++;
      }
   }
}
