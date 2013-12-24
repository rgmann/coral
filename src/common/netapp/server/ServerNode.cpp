#include <stdio.h>
#include <string.h>
#include "ServerNode.h"

using namespace liber::net;
using namespace liber::netapp;

//------------------------------------------------------------------------------
ServerNode::ServerNode()
{
  mpRxThread = NULL;
  mpWorkerThread = NULL;
  mpTxThread = NULL;
  mnTimeoutMs = MaxRecvTimeoutMs;
}

//------------------------------------------------------------------------------
ServerNode::~ServerNode()
{
  if (mpRxThread)
  {
    delete mpRxThread;
    mpRxThread = NULL;
  }

  if (mpWorkerThread)
  {
    delete mpWorkerThread;
    mpWorkerThread = NULL;
  }

  if (mpTxThread)
  {
    delete mpTxThread;
    mpTxThread = NULL;
  }
}

//------------------------------------------------------------------------------
bool  ServerNode::addWorker(ServerWorker* pWorker)
{
  return mRxQueue.push(pWorker, 500);
}

//------------------------------------------------------------------------------
bool  ServerNode::start()
{
  if (!mTxQueue.initialize(10))
  {
    printf("Failed to create TX queue!\r\n");
    return false;
  }

  if (!mWorkQueue.initialize(10))
  {
    printf("Failed to create work queue!\r\n");
    return false;
  }
   
  if (!mRxQueue.initialize(10))
  {
    printf("Failed to create RX queue!\r\n");
    return false;
  }
   
  mpTxThread = Thread::Create(txThreadFunc, this);
   
  if (mpTxThread == NULL)
  {
    printf("Failed to create TX thread!\r\n");
    return false;
  }
   
  mpWorkerThread = Thread::Create(workThreadFunc, this);
   
  if (mpWorkerThread == NULL)
  {
    printf("Failed to create worker thread!\r\n");
    return false;
  }
   
  mpRxThread = Thread::Create(rxThreadFunc, this);
   
  if (mpRxThread == NULL)
  {
    printf("Failed to create RX thread!\r\n");
    return false;
  }
   
  return true;
}

//------------------------------------------------------------------------------
bool  ServerNode::stop()
{
  printf("ServerNode::stop: Stopping node\n");
   
  if (mpRxThread)
  {
    mpRxThread->stop();
    mpRxThread->join();
  }
   
  if (mpWorkerThread)
  {
    mpWorkerThread->stop();
    mpWorkerThread->join();
  }
   
  if (mpTxThread)
  {
    mpTxThread->stop();
    mpTxThread->join();
  }
   
  cleanup();
   
  return true;
}

//------------------------------------------------------------------------------
void  ServerNode::setRecvTimeout(ui32 timeouMs)
{
  mnTimeoutMs = timeouMs;
}

//------------------------------------------------------------------------------
void ServerNode::rxThreadFunc(ThreadArg* pArg)
{
  ((ServerNode*)pArg->pUserData)->rxThread(pArg);
}

//------------------------------------------------------------------------------
bool ServerNode::recvPacket(ServerWorker* pWorker)
{
  bool  lbSuccess  = true;
  ui32  lnBytesRecvd  = 0;

  NetAppPacket::Data lPacketHeader;
   
  TcpSocket* lpSocket = pWorker->socket();

  // Each worker has a standard application specific header.  Each
  // packet should be preceded by a header and the header should
  // indicate the length of the following data.
  lnBytesRecvd  = lpSocket->read((char*)&lPacketHeader, sizeof(lPacketHeader), 20);
  if (lnBytesRecvd != sizeof(lPacketHeader))
  {      
    return false;
  }
   
  // Pass the header to the worker so that it can parse the
  // expected payload length.
  NetAppPacket* lpPacket = new NetAppPacket();
  lpPacket->allocate(lPacketHeader);
 
  if (lPacketHeader.length > 0)
  {
    lnBytesRecvd = lpSocket->read((char*)lpPacket->dataPtr(), lPacketHeader.length, 0);
      
    if (lnBytesRecvd != lPacketHeader.length)
    {
      lbSuccess = false;
    }
  }
   
  if (lbSuccess)
  {
    pWorker->put(lpPacket);
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
void ServerNode::rxThread(ThreadArg* pArg)
{
  bool           lbSuccess  = false;
  ServerWorker*  lpWorker   = NULL;
   
  printf("ServerNode::rxThread: Started!\n");
   
  while (!pArg->stopSignalled())
  {
    if (!mRxQueue.pop(lpWorker, 500))
    {
      continue;
    }
      
    // Get the worker's socket
    if (lpWorker == NULL)
    {
      continue;
    }
 
    lbSuccess = recvPacket(lpWorker);
         
    // Release the socket so that it can be used by the TX thread.
    if (lbSuccess)
    {
      // Push the worker into the work queue
      mWorkQueue.push(lpWorker, 500);
    }
    else
    {
      if (lpWorker->elapseMsSinceRecv() > mnTimeoutMs)
      {
        printf("ServerNode: Removing worker\n");
        delete lpWorker;
        lpWorker = NULL;
      }
      else
      {
        mRxQueue.push(lpWorker, 500);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ServerNode::txThreadFunc(ThreadArg* pArg)
{
  ((ServerNode*)pArg->pUserData)->txThread(pArg);
}

//------------------------------------------------------------------------------
void ServerNode::txThread(ThreadArg* pArg)
{
  TcpSocket*  lpSocket = NULL;
  ServerWorker* lpWorker = NULL;

  printf("ServerNode::txThread: Started!\n");
   
  while (!pArg->stopSignalled())
  {
    if (!mTxQueue.pop(lpWorker, 500))
    {
      continue;
    }
      
    // Get the worker's socket
    if (lpWorker == NULL)
    {
      printf("ServerNode::txThread: null\n");
      continue;
    }

    lpSocket = lpWorker->socket();

    // Each worker has a standard application specific header.  Each
    // packet should be preceded by a header and the header should
    // indicate the length of the following data.
    
    NetAppPacket* lpPacket = lpWorker->get();
    if (lpPacket == NULL)
    {         
      // Push the worker back into the Rx queue
      mRxQueue.push(lpWorker, 500);
         
      continue;
    }
         
    // Send the message
    
    lpSocket->write((char*)lpPacket->basePtr(), lpPacket->allocatedSize());
      
    // Release the socket so that it can be used by the TX thread.
    //lpWorker->releaseSocket();
      
    delete lpPacket;

    // Push the worker back into the Rx queue
    mRxQueue.push(lpWorker, 500);
  }
}

//------------------------------------------------------------------------------
void ServerNode::workThreadFunc(ThreadArg* pArg)
{
  ((ServerNode*)pArg->pUserData)->workThread(pArg);
}

//------------------------------------------------------------------------------
void ServerNode::workThread(ThreadArg* pArg)
{   
  ServerWorker* lpWorker = NULL;

  while (!pArg->stopSignalled())
  {
    if (!mWorkQueue.pop(lpWorker, 500))
    {
      continue;
    }
      
    // Get the worker's socket
    if (lpWorker == NULL)
    {
      continue;
    }
         
    lpWorker->work(100);

    // Push the worker back into the Rx queue
    mTxQueue.push(lpWorker, 500);
  }
}

//------------------------------------------------------------------------------
void ServerNode::cleanup()
{
  ServerWorker* lpWorker = NULL;

  while (mWorkQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      printf("ServerNode::mWorkQueue\n");
      delete lpWorker;
      lpWorker = NULL;
    }
  }
   
  while (mRxQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      printf("ServerNode::mRxQueue\n");
      delete lpWorker;
      lpWorker = NULL;
    }
  }
   
  while (mTxQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      printf("ServerNode::mTxQueue\n");
      delete lpWorker;
      lpWorker = NULL;
    }
  }
}

