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
  mRecvTs.sample();
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
ui32 ServerWorker::elapseMsSinceRecv()
{
  Timestamp lTs;
   
  lTs.sample();
   
  return (ui32)lTs.diffInMs(mRecvTs);
}

//------------------------------------------------------------------------------
bool ServerWorker::put(NetAppPacket* pPacket)
{
  mRecvTs.sample();
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
void ServerWorker::sampleRecvTime()
{
  mRecvTs.sample();
}

//------------------------------------------------------------------------------
void ServerWorker::work(int nSlotTimeUs)
{
  mRouter.routeFromQueue(nSlotTimeUs);
}


