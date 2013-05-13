#include "ServerWorker.h"

//------------------------------------------------------------------------------
ServerWorker::ServerWorker(TcpSocket* pSocket)
{
   m_pSocket = pSocket;
   m_RecvTs.sample();
}

//------------------------------------------------------------------------------
ServerWorker::~ServerWorker()
{
   if (m_pSocket)
   {
      printf("ServerWorker closing socket\n");
      m_pSocket->closeSocket();
      
      delete m_pSocket;
      m_pSocket = NULL;
   }
}

//------------------------------------------------------------------------------
bool ServerWorker::initialize()
{
   bool l_bSuccess = true;
   
   if (!m_InQ.initialize(10))
   {
      l_bSuccess = false;
   }
   
   if (!m_OutQ.initialize(10))
   {
      l_bSuccess = false;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
TcpSocket*  ServerWorker::socket()
{
   return m_pSocket;
}

//------------------------------------------------------------------------------
bool ServerWorker::lockSocket(ui32 timeoutMs)
{
   //return m_SocketLock.lock(timeoutMs);
   return true;
}

//------------------------------------------------------------------------------
void ServerWorker::releaseSocket()
{
   //m_SocketLock.unlock();
}

//------------------------------------------------------------------------------
ui32  ServerWorker::elapseMsSinceRecv()
{
   Timestamp l_Ts;
   
   l_Ts.sample();
   
   return (ui32)l_Ts.diffInMs(m_RecvTs);
}

//------------------------------------------------------------------------------
bool ServerWorker::processMsg(const char* pMsg, ui32 nMsgLenBytes)
{
   sampleRecvTime();
   return true;
}

//------------------------------------------------------------------------------
void ServerWorker::sampleRecvTime()
{
   m_RecvTs.sample();
}

//------------------------------------------------------------------------------
void ServerWorker::pushRx(GenericPacket* pPkt)
{
   m_InQ.push(pPkt, 500);
}

//------------------------------------------------------------------------------
bool ServerWorker::popRx(GenericPacket** pPkt)
{
   return m_InQ.pop(*pPkt, 0);
}

//------------------------------------------------------------------------------
void ServerWorker::pushTx(GenericPacket* pPkt)
{
   m_OutQ.push(pPkt, 500);
}

//------------------------------------------------------------------------------
bool ServerWorker::popTx(GenericPacket** pPkt)
{
   return m_OutQ.pop(*pPkt, 0);
}

