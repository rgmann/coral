#include <stdio.h>
#include <string.h>
#include <iostream>
#include "ServerNode.h"

using namespace liber::net;
using namespace liber::netapp;

//------------------------------------------------------------------------------
ServerNode::ServerNode()
: mpRxThread(NULL)
, mpWorkerThread(NULL)
, mpTxThread(NULL)
, mnTimeoutMs(MaxRecvTimeoutMs)
{
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

    std::cout << "ServerNode::stop: Shutdown receive thread." << std::endl;
  }
   
  if (mpWorkerThread)
  {
    mpWorkerThread->stop();
    mpWorkerThread->join();
    delete mpWorkerThread;
    mpWorkerThread = NULL;

    std::cout << "ServerNode::stop: Shutdown worker thread." << std::endl;
  }

  if (mpTxThread)
  {
    mpTxThread->stop();
    mpTxThread->join();
    delete mpTxThread;
    mpTxThread = NULL;

    std::cout << "ServerNode::stop: Shutdown transmit thread." << std::endl;
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
          std::cout << "ServerNode::recvPacket: type = " << lPacketHeader.type
                    << ", length = " << lPacketHeader.length << std::endl;

          if (!pWorker->put(lpPacket))
          {
            std::cout << "ServerNode::recvPacket: "
                      << "Failed to send packet to worker." << std::endl;
            delete lpPacket;
          }
        }
        else
        {
          std::cout << "ServerNode::recvPacket: "
                    << " Timed out receiving packet body "
                    << "(expected = " << lPacketHeader.length
                    << ", received = " << lStatus.byteCount
                    << ")" << std::endl;
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
          if (!mWorkQueue.push(lpWorker, 500))
          {
            std::cout << "ServerNode::rxThread: "
                        << "Failed to put worker in the work queue."
                        << std::endl;
          }
        }
        else if (lStatus == SocketTimeout)
        {
          // Remove inactive workers.  Workers are considered inactive if
          // the client hasn't sent anything in a while
          //if (lpWorker->elapseMsSinceRecv() > mnTimeoutMs)
          if (!lpWorker->status().isActive())
          {
            removeWorker(lpWorker);
          }
          else
          {
            if (!mRxQueue.push(lpWorker, 500))
            {
              std::cout << "ServerNode::rxThread: "
                        << "Failed to put worker back in the receive queue."
                        << std::endl;
            }
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
        if (lpPacket)
        {
          // Send the message
          lpSocket->write(lStatus,
                          (char*)lpPacket->basePtr(),
                          lpPacket->allocatedSize());
          delete lpPacket;
        }


        // Push the worker back into the Rx queue
        if (!mRxQueue.push(lpWorker, 500))
        {
          std::cout << "ServerNode::txThread: "
                    << "Failed to put worker into receive queue."
                    << std::endl;
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
        std::cout << "ServerNode::workThread: Starting work interval."
                  << std::endl;
        lpWorker->work(100);

        // Push the worker back into the Rx queue
        if (!mTxQueue.push(lpWorker, 500))
        {
          std::cout << "ServerNode::workThread: "
                    << "Failed to add worker to transmit queue." << std::endl;
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void ServerNode::cleanup()
{
  ServerWorker* lpWorker = NULL;

  std::cout << "ServerNode::cleanup: Emptying queues." << std::endl;

  while (mWorkQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      removeWorker(lpWorker);
      lpWorker = NULL;
    }
  }
  std::cout << "ServerNode::cleanup: Emptied work queue." << std::endl;
   
  while (mRxQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      removeWorker(lpWorker);
      lpWorker = NULL;
    }
  }
  std::cout << "ServerNode::cleanup: Emptied receive queue." << std::endl;
   
  while (mTxQueue.pop(lpWorker,20))
  {
    if (lpWorker)
    {
      removeWorker(lpWorker);
      lpWorker = NULL;
    }
  }
  std::cout << "ServerNode::cleanup: Emptied transmit queue." << std::endl;
}

//-----------------------------------------------------------------------------
void ServerNode::removeWorker(ServerWorker* pWorker)
{
  printf("ServerNode: Removing worker\n");

  pWorker->shutdown();
  delete pWorker;
}

