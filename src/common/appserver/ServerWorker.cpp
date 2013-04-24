#include "ServerWorker.h"

//------------------------------------------------------------------------------
ServerWorker::ServerWorker(TcpSocket* pSocket)
{
   m_pSocket = pSocket;
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
   return m_SocketLock.lock(timeoutMs);
}

//------------------------------------------------------------------------------
void ServerWorker::releaseSocket()
{
   //m_SocketLock.unlock();
}

//------------------------------------------------------------------------------
void ServerWorker::sampleRecvTime()
{
   m_RecvTs.sample();
}

//------------------------------------------------------------------------------
i64  ServerWorker::lastRecvTime()
{
   Timestamp l_Ts;
   
   l_Ts.sample();
   
   return m_RecvTs.diffInMs(l_Ts);
}

//------------------------------------------------------------------------------
void ServerWorker::pushRx(GenericPacket* pPkt)
{
   m_InQ.push(pPkt, 500);
}
//void ServerWorker::pushRx(const GenericPacket& pkt)
//{
//   m_InQ.push(pkt, 500);
//}

//------------------------------------------------------------------------------
bool ServerWorker::popRx(GenericPacket** pPkt)
{
   return m_InQ.pop(*pPkt, 0);
}
//bool ServerWorker::popRx(GenericPacket &pkt)
//{
//   return m_InQ.pop(pkt, 0);
//}

//------------------------------------------------------------------------------
void ServerWorker::pushTx(GenericPacket* pPkt)
{
   m_OutQ.push(pPkt, 500);
}
//void ServerWorker::pushTx(const GenericPacket &pkt)
//{
//   m_OutQ.push(pkt, 500);
//}

//------------------------------------------------------------------------------
bool ServerWorker::popTx(GenericPacket** pPkt)
{
   return m_OutQ.pop(*pPkt, 0);
}
//bool ServerWorker::popTx(GenericPacket &pkt)
//{
//   return m_OutQ.pop(pkt, 0);
//}

