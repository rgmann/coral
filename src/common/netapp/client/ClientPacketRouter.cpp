#include <string.h>
#include "NetAppPacket.h"
#include "ClientPacketRouter.h"

using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
ClientPacketRouter::ClientPacketRouter(Socket& rSocket)
: mrSocket(rSocket)
, mpTxThread(NULL)
, mpRxThread(NULL)
, mnReadTimeoutMs(100)
{
}

//----------------------------------------------------------------------------
ClientPacketRouter::~ClientPacketRouter()
{
  if (mpTxThread)
  {
    delete mpTxThread;
  }

  if (mpRxThread)
  {
    delete mpRxThread;
  }
}

//-----------------------------------------------------------------------------
bool ClientPacketRouter::addSubscriber(int               subscriberId,
                                       PacketSubscriber* pSubscriber)
{
  bool lbSuccess = (mSubscriberTable.count(subscriberId) != 0);

  lbSuccess &= (pSubscriber != NULL);
  if (lbSuccess)
  {
    lbSuccess = mTableLock.lock();
    if (lbSuccess)
    {
      pSubscriber->setId(subscriberId);
      pSubscriber->setOutputQueue(&mTxQueue);
      mSubscriberTable.insert(std::make_pair(subscriberId, pSubscriber));
      lbSuccess = true;
      mTableLock.unlock();
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
PacketSubscriber* ClientPacketRouter::removeSubscriber(int subscriberId)
{
  PacketSubscriber* lpSubscriber = NULL;
  bool lbSuccess = (mSubscriberTable.count(subscriberId) != 0);

  lbSuccess &= (lpSubscriber != NULL);
  if (lbSuccess)
  {
    lbSuccess = mTableLock.lock();
    if (lbSuccess)
    {
      lpSubscriber = mSubscriberTable.find(subscriberId)->second;
      mSubscriberTable.erase(subscriberId);
      lbSuccess = true;
      mTableLock.unlock();
    }
  }

  return lpSubscriber;
}

//-----------------------------------------------------------------------------
bool ClientPacketRouter::start(ui32 txCapacity)
{
  bool lbSuccess = true;

  if (!mTxQueue.initialize(txCapacity))
  {
    lbSuccess = false;
  }

  mpTxThread = Thread::Create(TxThreadEntry, this);
  if (mpTxThread == NULL)
  {
    printf("Failed to create worker thread!\r\n");
    lbSuccess = false;
  }

  mpRxThread = Thread::Create(RxThreadEntry, this);
  if (mpRxThread == NULL)
  {
    printf("Failed to create worker thread!\r\n");
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
  }

  if (mpRxThread)
  {
    mpRxThread->stop();
    mpRxThread->join();
    delete mpRxThread;
  }
}

//-----------------------------------------------------------------------------
Socket& ClientPacketRouter::socket()
{
  return mrSocket;
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::setReadTimeout(int nTimeoutMs)
{
  mnReadTimeoutMs = nTimeoutMs;
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
void ClientPacketRouter::txThreadRun(ThreadArg* pArg)
{
  i32 lnBytes = 0;
  NetAppPacket::Data lPacketHeader;
  NetAppPacket lPacket;

  while (!pArg->stopSignalled())
  {
    lnBytes = mrSocket.read((char*)&lPacketHeader, sizeof(lPacketHeader), mnReadTimeoutMs);
    if (lnBytes == sizeof(lPacketHeader))
    {
      if (lPacket.allocate(lPacketHeader))
      {
        lnBytes = mrSocket.read((char*)lPacket.dataPtr(), lPacketHeader.length, mnReadTimeoutMs);
        if (lnBytes == lPacketHeader.length)
        {
          mTableLock.lock();
          bool lbExists = (mSubscriberTable.count(lPacketHeader.type) != 0);

          if (lbExists)
          {
            PacketSubscriber* lpSubscriber = NULL;
            lpSubscriber = mSubscriberTable.find(lPacketHeader.type)->second;

            if (lpSubscriber)
            {
              lpSubscriber->put((char*)lPacket.dataPtr(), lPacketHeader.length);
            }
          }

          mTableLock.unlock();
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::rxThreadRun(ThreadArg* pArg)
{
  NetAppPacket* lpPacket = NULL;

  while (!pArg->stopSignalled())
  {
    if (mTxQueue.pop(lpPacket, 100))
    {
      i32 lnBytes = mrSocket.write((char*)lpPacket->basePtr(), 
                                   lpPacket->allocatedSize());
      if (lnBytes != lpPacket->allocatedSize())
      {
        printf("Failed to send packet\n");
      }

      delete lpPacket;
      lpPacket = NULL;
    }
  }
}

