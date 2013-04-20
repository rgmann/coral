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
      delete[] m_pSocket;
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
TcpSocket*  ServerWorker::socket(ui32 nTimeoutMs)
{
   TcpSocket*  pRetSocket = NULL;
   
   if (m_SocketLock.lock())
   {
      pRetSocket = m_pSocket;
      
      m_SocketLock.unlock();
   }
   
   return pRetSocket;
}

//------------------------------------------------------------------------------
void ServerWorker::releaseSocket()
{
   m_SocketLock.unlock();
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

