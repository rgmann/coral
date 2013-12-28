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
  stop();
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
    delete mpRxThread;
    mpRxThread = NULL;
  }
   
  if (mpWorkerThread)
  {
    mpWorkerThread->stop();
    mpWorkerThread->join();
    delete mpWorkerThread;
    mpWorkerThread = NULL;
  }

  if (mpTxThread)
  {
    mpTxThread->stop();
    mpTxThread->join();
    delete mpTxThread;
    mpTxThread = NULL;
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
SocketError ServerNode::recvPacket(ServerWorker* pWorker)
{
//  bool  lbSuccess  = false;
  //int   lnBytesRecvd  = 0;
  SocketStatus lStatus;

  NetAppPacket::Data lPacketHeader;
   
  TcpSocket* lpSocket = pWorker->socket();

  // Each worker has a standard application specific header.  Each
  // packet should be preceded by a header and the header should
  // indicate the length of the following data.
  lpSocket->read(lStatus, (char*)&lPacketHeader, sizeof(lPacketHeader), 20);

  if ((lStatus.status == SocketOk) && (lStatus.byteCount == sizeof(lPacketHeader)))
  {
    if (lPacketHeader.length > 0)
    {
      // Pass the header to the worker so that it can parse the
      // expected payload length.
      NetAppPacket* lpPacket = new NetAppPacket();
      if (lpPacket->allocate(lPacketHeader))
      {
        lpSocket->read(lStatus, (char*)lpPacket->dataPtr(),
                       lPacketHeader.length, 20);
    
        if (lStatus.byteCount == lPacketHeader.length)
        {
          if (!pWorker->put(lpPacket))
          {
            delete lpPacket;
          }
        }
      }
      else
      {
        printf("ServerNode::recvPacket: Failed to allocate NetAppPacket!\n");
      }
    }
  }
   
  return lStatus.status;
}

//------------------------------------------------------------------------------
void ServerNode::rxThread(ThreadArg* pArg)
{
  ServerWorker*  lpWorker   = NULL;
   
  printf("ServerNode::rxThread: Started!\n");
   
  while (!pArg->stopSignalled())
  {
    lpWorker = NULL;

    if (mRxQueue.pop(lpWorker, 500))
    {
      if (lpWorker)
      {
        // Release the socket so that it can be used by the TX thread.
        SocketError lStatus = recvPacket(lpWorker);
        if (lStatus == SocketOk)
        {
          // Push the worker into the work queue
          mWorkQueue.push(lpWorker, 500);
        }
        else if (lStatus == SocketTimeout)
        {
          // Remove in active workers.  Workers are considered inactive if
          // the client hasn't sent anything in a while
          if (lpWorker->elapseMsSinceRecv() > mnTimeoutMs)
          {
            removeWorker(lpWorker);
          }
          else
          {
            mRxQueue.push(lpWorker, 500);
          }
        }
        else
        {
          removeWorker(lpWorker);
        }
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
  SocketStatus  lStatus;
  TcpSocket*    lpSocket = NULL;
  ServerWorker* lpWorker = NULL;

  printf("ServerNode::txThread: Started!\n");
   
  while (!pArg->stopSignalled())
  {
    lpWorker = NULL;

    if (mTxQueue.pop(lpWorker, 500))
    {
      if (lpWorker)
      {
        lpSocket = lpWorker->socket();

        NetAppPacket* lpPacket = lpWorker->get();
        if (lpPacket == NULL)
        {
          // Push the worker back into the Rx queue
          mRxQueue.push(lpWorker, 500);
        }
        else
        {
          // Send the message
          lpSocket->write(lStatus,
                          (char*)lpPacket->basePtr(),
                          lpPacket->allocatedSize());
          delete lpPacket;

          // Push the worker back into the Rx queue
          mRxQueue.push(lpWorker, 500);
        }
      }
    }
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
    lpWorker = NULL;

    if (mWorkQueue.pop(lpWorker, 500))
    {
      if (lpWorker)
      {
        lpWorker->work(100);

        // Push the worker back into the Rx queue
        mTxQueue.push(lpWorker, 500);
      }
    }
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
      removeWorker(lpWorker);
      lpWorker = NULL;
    }
  }
   
  while (mRxQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      removeWorker(lpWorker);
      lpWorker = NULL;
    }
  }
   
  while (mTxQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      removeWorker(lpWorker);
      lpWorker = NULL;
    }
  }
}

//-----------------------------------------------------------------------------
void ServerNode::removeWorker(ServerWorker* pWorker)
{
  printf("ServerNode: Removing worker\n");

  pWorker->shutdown();
  delete pWorker;
}

