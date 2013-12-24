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
  if (mpSocket)
  {
    printf("ServerWorker closing socket\n");
    mpSocket->disconnect();
      
    delete mpSocket;
    mpSocket = NULL;
  }
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
   
  return lbSuccess;
}

//------------------------------------------------------------------------------
TcpSocket* ServerWorker::socket()
{
  return mpSocket;
}

//------------------------------------------------------------------------------
/*bool ServerWorker::lockSocket(ui32 timeoutMs)
{
  //return m_SocketLock.lock(timeoutMs);
  return true;
}*/

//------------------------------------------------------------------------------
/*void ServerWorker::releaseSocket()
{
  //m_SocketLock.unlock();
}*/

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

//------------------------------------------------------------------------------
/*void ServerWorker::pushRx(GenericPacket* pPkt)
{
  mInQ.push(pPkt, 500);
}*/

//------------------------------------------------------------------------------
/*bool ServerWorker::popRx(GenericPacket** pPkt)
{
  return mInQ.pop(*pPkt, 0);
}*/

//------------------------------------------------------------------------------
/*void ServerWorker::pushTx(GenericPacket* pPkt)
{
  mOutQ.push(pPkt, 500);
}*/

//------------------------------------------------------------------------------
/*bool ServerWorker::popTx(GenericPacket** pPkt)
{
  return mOutQ.pop(*pPkt, 0);
}*/

