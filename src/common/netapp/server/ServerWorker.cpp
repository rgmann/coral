#include "ServerWorker.h"

using namespace liber::net;
using namespace liber::netapp;

//------------------------------------------------------------------------------
ServerWorker::ServerWorker(TcpSocket* pSocket)
: mpSocket(pSocket)
, mRouter(&mInQueue, &mOutQueue)
, mEnqueueTimeoutMs(DefaultEnqueueTimeoutMs)
, mDequeueTimeoutMs(DefaultDequeueTimeoutMs)
{
}

//------------------------------------------------------------------------------
ServerWorker::~ServerWorker()
{
}

//------------------------------------------------------------------------------
bool ServerWorker::initialize()
{
  bool lbSuccess = true;
   
  if (!mInQueue.initialize())
  {
    lbSuccess = false;
  }
   
  if (!mOutQueue.initialize())
  {
    lbSuccess = false;
  }

  // Register the keepalive subscriber.
  lbSuccess &= mRouter.addSubscriber(ConnectionStatus::SubscriberId,
                                     &mConnectionStatus);

  //
  lbSuccess &= setup();
   
  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool ServerWorker::setup()
{
  printf("ServerWorker::setup() is unimplemented\n");
  return true;
}

//-----------------------------------------------------------------------------
void ServerWorker::shutdown()
{
  if (mpSocket)
  {
    printf("ServerWorker closing socket\n");
    mpSocket->disconnect();

    delete mpSocket;
    mpSocket = NULL;
  }

  NetAppPacket* lpPacket = NULL;
  while (mInQueue.pop(lpPacket, 5))
  {
    if (lpPacket) delete lpPacket;
    lpPacket = NULL;
  }

  while (mOutQueue.pop(lpPacket, 5))
  {
    if (lpPacket) delete lpPacket;
    lpPacket = NULL;
  }

  // Unregister the keepalive subscriber.
  mRouter.removeSubscriber(ConnectionStatus::SubscriberId);

  teardown();
}

//------------------------------------------------------------------------------
void ServerWorker::teardown()
{
  printf("ServerWorker::teardown() is unimplemented\n");
}

//------------------------------------------------------------------------------
TcpSocket* ServerWorker::socket()
{
  return mpSocket;
}

//------------------------------------------------------------------------------
ServerPacketRouter& ServerWorker::router()
{
  return mRouter;
}

//------------------------------------------------------------------------------
const ConnectionStatus& ServerWorker::status()
{
  return mConnectionStatus;
}

//------------------------------------------------------------------------------
bool ServerWorker::put(NetAppPacket* pPacket)
{
  mConnectionStatus.sampleRecvTime();
  return mInQueue.push(pPacket, mEnqueueTimeoutMs);
}

//------------------------------------------------------------------------------
NetAppPacket* ServerWorker::get()
{
  NetAppPacket* lpPacket = NULL;
  mOutQueue.pop(lpPacket, mDequeueTimeoutMs);
  return lpPacket;
}

//------------------------------------------------------------------------------
void ServerWorker::work(int nSlotTimeUs)
{
  mRouter.routeFromQueue(nSlotTimeUs);
}


