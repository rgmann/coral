#include <string.h>
#include "NetAppPacket.h"
#include "ClientPacketRouter.h"

using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
ClientPacketRouter::ClientPacketRouter(Socket& rSocket)
: PacketRouter(&mTxQueue)
, mrSocket(rSocket)
, mpTxThread(NULL)
, mpRxThread(NULL)
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
    mpTxThread = NULL;
  }

  if (mpRxThread)
  {
    mpRxThread->stop();
    mpRxThread->join();
    delete mpRxThread;
    mpRxThread = NULL;
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
  SocketStatus  lStatus;
  NetAppPacket* lpPacket = NULL;

  while (!pArg->stopSignalled())
  {
    if (mTxQueue.pop(lpPacket, 100))
    {
      mrSocket.write(lStatus, (char*)lpPacket->basePtr(), lpPacket->allocatedSize());

      // Check that the packet was successfully sent.
      if (lStatus.status != SocketOk || lStatus.byteCount != lpPacket->allocatedSize())
      {
        printf("Failed to send packet\n");
      }

      delete lpPacket;
      lpPacket = NULL;
    }
  }
}

//-----------------------------------------------------------------------------
void ClientPacketRouter::rxThreadRun(ThreadArg* pArg)
{
  SocketStatus       lStatus;
  NetAppPacket::Data lPacketHeader;
  NetAppPacket       lPacket;

  while (!pArg->stopSignalled())
  {
    mrSocket.read(lStatus, (char*)&lPacketHeader, sizeof(lPacketHeader), mnReadTimeoutMs);

    if (lStatus.byteCount == sizeof(lPacketHeader))
    {
      if (lPacket.allocate(lPacketHeader))
      {
        mrSocket.read(lStatus, (char*)lPacket.dataPtr(),
                      lPacketHeader.length, mnReadTimeoutMs);

        if (lStatus.byteCount == lPacketHeader.length)
        {
          PacketSubscriber* lpSubscriber = getSubscriber(lPacketHeader.type);
          if (lpSubscriber)
          {
            lpSubscriber->put((char*)lPacket.dataPtr(), lPacketHeader.length);
          }
        }
      }
    }
  }
}


