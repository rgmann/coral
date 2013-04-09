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
