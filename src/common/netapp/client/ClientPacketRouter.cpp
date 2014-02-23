#include <string.h>
#include <iostream>
#include "NetAppPacket.h"
#include "ClientPacketRouter.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
ClientPacketRouter::ClientPacketRouter()
: PacketRouter(&mTxQueue)
, mbKeepaliveEnabled(true)
, mpTxThread(NULL)
, mpRxThread(NULL)
, mpRouterThread(NULL)
, mnReadTimeoutMs(100)
{
}

//----------------------------------------------------------------------------
ClientPacketRouter::~ClientPacketRouter()
{
  stop();
}

//-----------------------------------------------------------------------------
bool ClientPacketRouter::start(ui32 txCapacity)
{
  bool lbSuccess = true;

  if (!mTxQueue.initialize(txCapacity))
  {
    lbSuccess = false;
  }

  if (!mRxQueue.initialize(txCapacity))
  {
    lbSuccess = false;
  }

  // Register the connection status subscriber.
  lbSuccess &= addSubscriber(ConnectionStatus::SubscriberId,
                             &mConnectionStatus);

  mpRouterThread = Thread::Create(RouterThreadEntry, this);
  if (mpRouterThread == NULL)
  {
    std::cout << "Failed to create Router Thread." << std::endl;
    lbSuccess = false;
  }

  mpTxThread = Thread::Create(TxThreadEntry, this);
  if (mpTxThread == NULL)
  {
    std::cout << "Failed to create Packet Transmission Thread." << std::endl;
    lbSuccess = false;
  }

  mpRxThread = Thread::Create(RxThreadEntry, this);
  if (mpRxThread == NULL)
  {
    std::cout << "Failed to create Packet Receive Thread." << std::endl;
    lbSuccess = false;
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::stop()
{
  if (mpTxThread)
  {
    mpTxThread->stop();
    mpTxThread->join();
    delete mpTxThread;
    mpTxThread = NULL;
  }

  if (mpRxThread)
  {
    mpRxThread->stop();
    mpRxThread->join();
    delete mpRxThread;
    mpRxThread = NULL;
  }

  if (mpRouterThread)
  {
    mpRouterThread->stop();
    mpRouterThread->join();
    delete mpRouterThread;
    mpRouterThread = NULL;
  }

  // Unregister the connection status subscriber.
  removeSubscriber(ConnectionStatus::SubscriberId);
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::setReadTimeout(int nTimeoutMs)
{
  mnReadTimeoutMs = nTimeoutMs;
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::enableKeepalive(bool bEnable)
{
  mbKeepaliveEnabled = bEnable;
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::TxThreadEntry(ThreadArg* pArg)
{
  ((ClientPacketRouter*)pArg->pUserData)->txThreadRun(pArg);
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::RxThreadEntry(ThreadArg* pArg)
{
  ((ClientPacketRouter*)pArg->pUserData)->rxThreadRun(pArg);
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::RouterThreadEntry(ThreadArg* pArg)
{
  ((ClientPacketRouter*)pArg->pUserData)->routerThreadRun(pArg);
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::txThreadRun(ThreadArg* pArg)
{
  NetAppPacket* lpPacket = NULL;

  while (!pArg->stopSignalled())
  {
    // If keepalives are enabled, check whether it is time to send a keepalive.
    if (mbKeepaliveEnabled)
    {
      if (Timestamp::Now().diffInMs(mConnectionStatus.lastKeepaliveSendTime()) > 1000)
      {
        mConnectionStatus.sendKeepalive();
      }
    }

    // TODO: Should this peek or just drop the packet?
    if (mTxQueue.pop(lpPacket, 100) && lpPacket)
    {
      if (writePacket(*lpPacket))
      {
        // TODO: Increment TX success count.
      }
      else
      {
        // TODO: Increment TX failure count.
        std::cout << "ClientPacketRouter: Failed to send packet.\n"
                  << std::endl;
      }

      delete lpPacket;
      lpPacket = NULL;
    }
  }
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::rxThreadRun(ThreadArg* pArg)
{
  NetAppPacket* lpPacket = NULL;

  while (!pArg->stopSignalled())
  {
    lpPacket = new NetAppPacket();

    if (readPacket(*lpPacket, mnReadTimeoutMs))
    {
      if (mRxQueue.push(lpPacket))
      {
        // TODO: Increment packet receive count.
        lpPacket = NULL;
      }
      else
      {
        // TODO: Increment packet drop count.
      }
    }

    if (lpPacket)
    {
       delete lpPacket;
       lpPacket = NULL;
    }
  }
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::routerThreadRun(ThreadArg* pArg)
{
  NetAppPacket* lpPacket = NULL;

  while (!pArg->stopSignalled())
  {
    if (mRxQueue.pop(lpPacket, 100) && lpPacket)
    {
      PacketSubscriber* lpSubscriber = getSubscriber(lpPacket->data()->type);
      if (lpSubscriber)
      {
        lpSubscriber->put((char*)lpPacket->dataPtr(), lpPacket->data()->length);
      }
    }

    lpPacket = NULL;
  }
}
