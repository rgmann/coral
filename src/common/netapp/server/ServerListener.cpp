#include <stdio.h>
#include "ServerListener.h"
#include "TcpServer.h"

using namespace liber::net;
using namespace liber::netapp;

//------------------------------------------------------------------------------
ServerListener::ServerListener()
{
  mpListenerThread = NULL;
  mpBalancerThread = NULL;
  mnMinNodeCount = 1;
}

//------------------------------------------------------------------------------
ServerListener::~ServerListener()
{
  if (mpListenerThread)
  {
    delete mpListenerThread;
    mpListenerThread = NULL;
  }
   
  if (mpBalancerThread)
  {
    delete mpBalancerThread;
    mpBalancerThread = NULL;
  }
}

//------------------------------------------------------------------------------
void ServerListener::setPort(ui32 port)
{
  mnServerPort = port;
}

//------------------------------------------------------------------------------
bool  ServerListener::start()
{
  // Initialize the accepted worker queue
  if (!mWorkerQueue.initialize(10))
  {
    printf("ServerListener::start: Failed to create worker queue.\n");
    return false;
  }
   
  mpListenerThread = Thread::Create(listenThreadFunc, this);
   
  if (mpListenerThread == NULL)
  {
    printf("ServerListener::start: Failed to create listener thread.\n");
    return false;
  }
   
  mpBalancerThread = Thread::Create(distribThreadFunc, this);
   
  if (mpBalancerThread == NULL)
  {
    printf("ServerListener::start: Failed to create listener thread.\n");
    return false;
  }
   
  return true;
}

//------------------------------------------------------------------------------
bool  ServerListener::stop()
{
  if (mpBalancerThread)
  {
    mpBalancerThread->stop();
    mpBalancerThread->join();
  }
   
  if (mpListenerThread)
  {
    mpListenerThread->stop();
    mpListenerThread->join();
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
  TcpServer   lTcpServer;
  TcpSocket*  lpSocket = NULL;
   
  ServerWorker*  lpWorker = NULL;
   
  // Start the server on the specified port
  printf("ServerListener: Starting server on port #%d\n", mnServerPort);
  lTcpServer.start(mnServerPort);
   
  // Main thread loop
  while (!pArg->stopSignalled())
  {
    lpSocket = lTcpServer.acceptClient(500);
      
    if (lpSocket == NULL)
    {
      continue;
    }
      
    // The inheriting class overrides the createWorker method to create
    // the appropiriate type of worker.
    printf("ServerListener: Received connection. Creating worker.\n");
    lpWorker = createWorker(lpSocket);
      
    // If the worker was successfully created pass the worker to the
    // distribution and load balancing thread.
    if (lpWorker)
    {
      lpWorker->initialize();
      distributeWorker(lpWorker);
    }
      
    // We're now done with the pointers, so reset them to NULL.
    lpWorker = NULL;
    lpSocket = NULL;
  }
   
  // Stop the TCP server
  lTcpServer.stop();
   
  printf("ServerListener::listenThread: Stopped!\n");
}

//------------------------------------------------------------------------------
bool ServerListener::distributeWorker(ServerWorker* pWorker)
{
  return mWorkerQueue.push(pWorker, 500);
}

//------------------------------------------------------------------------------
void  ServerListener::distribThreadFunc(ThreadArg* pArg)
{
  ((ServerListener*)pArg->pUserData)->distribThread(pArg);
}

//------------------------------------------------------------------------------
void  ServerListener::distribThread(ThreadArg* pArg)
{
  ui32  lnCurrentNode = 0;
  ServerWorker*  lpWorker = NULL;
  std::vector<ServerNode*>::iterator lVecIt;
   
  printf("ServerListener::distribThread: Started!\n");

  // Add and start the minimum number of nodes
  while (mvNodeList.size() < mnMinNodeCount)
  {
    ServerNode* lpNode = NULL;
      
    lpNode = new ServerNode();
    if (lpNode == NULL)
    {
      printf("ServerListener::listenThread: Failed to allocate node\n");
      break;
    }
      
    // Start the node
    lpNode->start();
      
    // Add the node to the node list
    mvNodeList.push_back(lpNode);
  }
   
  // Main thread loop
  while (!pArg->stopSignalled())
  {
    // Wait briefly for a new worker
    if (!mWorkerQueue.pop(lpWorker, 500)) continue;
      
    // If the worker is not NULL then we can add it to a server node.
    // Currently, round-robin is the only assignment policy.
    if (mvNodeList[lnCurrentNode] != NULL)
    {
      printf("ServerListener::distribThread: Adding worker\n");
      mvNodeList[lnCurrentNode]->addWorker(lpWorker);
         
      lnCurrentNode = (lnCurrentNode + 1) % mvNodeList.size();
    }
  }
   
  // Delete all nodes
  lVecIt = mvNodeList.begin();
  for (; lVecIt < mvNodeList.end(); ++lVecIt)
  {
    if (*lVecIt)
    {
      (*lVecIt)->stop();
         
      delete *lVecIt;
      *lVecIt = NULL;
    }
  }
   
  mvNodeList.clear();
   
  printf("ServerListener::distribThread: Stopped!\n");
}

