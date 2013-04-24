#include <stdio.h>
#include <string.h>
#include "ServerNode.h"

//------------------------------------------------------------------------------
ServerNode::ServerNode()
{
   m_pRxThread = NULL;
   m_pWorkerThread = NULL;
   m_pTxThread = NULL;
   m_nTimeoutMs = MaxRecvTimeoutMs;
}

//------------------------------------------------------------------------------
ServerNode::~ServerNode()
{
   if (m_pRxThread)
   {
      delete m_pRxThread;
      m_pRxThread = NULL;
   }
   
   if (m_pWorkerThread)
   {
      delete m_pWorkerThread;
      m_pWorkerThread = NULL;
   }
   
   if (m_pTxThread)
   {
      delete m_pTxThread;
      m_pTxThread = NULL;
   }

}

//------------------------------------------------------------------------------
bool  ServerNode::addWorker(ServerWorker* pWorker)
{
   pWorker->sampleRecvTime();
   
   return m_RxQueue.push(pWorker, 500);
}

//------------------------------------------------------------------------------
bool  ServerNode::start()
{
   if (!m_TxQueue.initialize(10))
   {
      printf("Failed to create TX queue!\r\n");
      return false;
   }
   
   if (!m_WorkQueue.initialize(10))
   {
      printf("Failed to create work queue!\r\n");
      return false;
   }
   
   if (!m_RxQueue.initialize(10))
   {
      printf("Failed to create RX queue!\r\n");
      return false;
   }
   
   m_pTxThread = Thread::Create(txThreadFunc, this);
   
   if (m_pTxThread == NULL)
   {
      printf("Failed to create TX thread!\r\n");
      return false;
   }
   
   m_pWorkerThread = Thread::Create(workThreadFunc, this);
   
   if (m_pWorkerThread == NULL)
   {
      printf("Failed to create worker thread!\r\n");
      return false;
   }
   
   m_pRxThread = Thread::Create(rxThreadFunc, this);
   
   if (m_pRxThread == NULL)
   {
      printf("Failed to create RX thread!\r\n");
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool  ServerNode::stop()
{
   printf("ServerNode::stop: Stopping node\n");
   
   if (m_pRxThread)
   {
      m_pRxThread->stop();
      m_pRxThread->join();
   }
   
   if (m_pWorkerThread)
   {
      m_pWorkerThread->stop();
      m_pWorkerThread->join();
   }
   
   if (m_pTxThread)
   {
      m_pTxThread->stop();
      m_pTxThread->join();
   }
   
   cleanup();
   
   return true;
}

//------------------------------------------------------------------------------
void  ServerNode::setRecvTimeout(ui32 timeouMs)
{
   m_nTimeoutMs = timeouMs;
}

//------------------------------------------------------------------------------
void ServerNode::rxThreadFunc(ThreadArg* pArg)
{
   ((ServerNode*)pArg->pUserData)->rxThread(pArg);
}

//------------------------------------------------------------------------------
bool ServerNode::recvPacket(ServerWorker* pWorker)
{
   bool  l_bSuccess  = true;
   char* l_pHeader   = NULL;
   char* l_pPkt      = NULL;
   
   ui32  l_nHeaderLen   = 0;
   ui32  l_nDataLen     = 0;
   ui32  l_nPktLen      = 0;
   ui32  l_nBytesRecvd  = 0;
   
   TcpSocket* l_pSocket = pWorker->socket();
   
   // Each worker has a standard application specific header.  Each
   // packet should be preceded by a header and the header should
   // indicate the length of the following data.
   l_nHeaderLen   = pWorker->headerSize();
   
   l_pHeader = new char[l_nHeaderLen];
   
   l_nBytesRecvd  = l_pSocket->recv(l_pHeader, l_nHeaderLen, 20);
   
   if (l_nHeaderLen != l_nBytesRecvd)
   {
      delete[] l_pHeader;
      l_pHeader = NULL;
      
      return false;
   }
   
   // Pass the header to the worker so that it can parse the
   // expected payload length.
   l_nDataLen = pWorker->getExpPayloadSize(l_pHeader);
   
   l_nPktLen = l_nHeaderLen + l_nDataLen;
   l_pPkt = new char[l_nPktLen];
   
   // Copy the header into the full packet
   memcpy(l_pPkt, l_pHeader, l_nHeaderLen);
   
   if (l_nDataLen > 0)
   {
      l_nBytesRecvd  = l_pSocket->recv(l_pPkt + l_nHeaderLen,
                                       l_nDataLen, 0);
      
      if (l_nBytesRecvd != l_nDataLen)
      {
         l_bSuccess = false;
      }
   }
   
   if (l_bSuccess)
   {
      pWorker->processMsg(l_pPkt, l_nPktLen);
   }
   
   delete[] l_pPkt;
   l_pPkt = NULL;
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
void ServerNode::rxThread(ThreadArg* pArg)
{
   bool           l_bSuccess  = false;
   ServerWorker*  l_pWorker   = NULL;
   
   printf("ServerNode::rxThread: Started!\n");
   
   while (!pArg->stopSignalled())
   {
      if (!m_RxQueue.pop(l_pWorker, 500))
      {
         continue;
      }
      
      // Get the worker's socket
      if (l_pWorker == NULL)
      {
         continue;
      }
         
      if (!l_pWorker->lockSocket(500))
      {
         // Put the worker back into the receive queue.
         m_RxQueue.push(l_pWorker, 500);
         
         continue;
      }
      
      l_bSuccess = recvPacket(l_pWorker);
         
      // Release the socket so that it can be used by the TX thread.
      l_pWorker->releaseSocket();
      
      if (l_bSuccess)
      {
         // Push the worker into the work queue
         m_WorkQueue.push(l_pWorker, 500);
      }
      else
      {
         delete l_pWorker;
         l_pWorker = NULL;
      }
   }
}

//------------------------------------------------------------------------------
void ServerNode::txThreadFunc(ThreadArg* pArg)
{
   ((ServerNode*)pArg->pUserData)->txThread(pArg);
}

//------------------------------------------------------------------------------
void ServerNode::txThread(ThreadArg* pArg)
{
   TcpSocket*  l_pSocket = NULL;
   ServerWorker* l_pWorker = NULL;
   
   ui32         l_nMsgLen   = 0;
   
   char*       l_pMsg    = NULL;
   
   bool  l_bMsgAvailable = false;
   
   printf("ServerNode::txThread: Started!\n");
   
   while (!pArg->stopSignalled())
   {
      if (!m_TxQueue.pop(l_pWorker, 500))
      {
         continue;
      }
//      printf("ServerNode::txThread: got worker\n");
      
      // Get the worker's socket
      if (l_pWorker == NULL)
      {
         printf("ServerNode::txThread: null\n");
         continue;
      }

      if (!l_pWorker->lockSocket(500))
      {
         //printf("ServerNode::txThread: Failed to get socket\n");
         
         // Put the worker back into the receive queue.
         m_TxQueue.push(l_pWorker, 500);
         
         continue;
      }
      
      l_pSocket = l_pWorker->socket();
//      printf("ServerNode::txThread: got socket\n");
      
      // Each worker has a standard application specific header.  Each
      // packet should be preceded by a header and the header should
      // indicate the length of the following data.
      l_bMsgAvailable = l_pWorker->getMsg(&l_pMsg, l_nMsgLen);
      if (!l_bMsgAvailable)
      {
         //printf("ServerNode::txThread: Failed to get message\n");
         
         // Push the worker back into the Rx queue
         m_RxQueue.push(l_pWorker, 500);
         
         continue;
      }
         
      // Send the message
//      printf("ServerNode::txThread: sending message - len = %u\n", l_nMsgLen);
      l_pSocket->send(l_pMsg, l_nMsgLen);
      
      // Release the socket so that it can be used by the TX thread.
      l_pWorker->releaseSocket();
      
      delete[] l_pMsg;
      l_pMsg = NULL;

      // Push the worker back into the Rx queue
      m_RxQueue.push(l_pWorker, 500);
   }
}

//------------------------------------------------------------------------------
void ServerNode::workThreadFunc(ThreadArg* pArg)
{
   ((ServerNode*)pArg->pUserData)->workThread(pArg);
}

//------------------------------------------------------------------------------
void ServerNode::workThread(ThreadArg* pArg)
{   
   ServerWorker* l_pWorker = NULL;
      
   while (!pArg->stopSignalled())
   {
      if (!m_WorkQueue.pop(l_pWorker, 500))
      {
         continue;
      }
         
      // Get the worker's socket
      if (l_pWorker == NULL)
      {
         continue;
      }
         
      l_pWorker->work();

      // Push the worker back into the Rx queue
      m_TxQueue.push(l_pWorker, 500);
   }
}

//------------------------------------------------------------------------------
void ServerNode::cleanup()
{
   ServerWorker* l_pWorker = NULL;
   
   while (m_WorkQueue.pop(l_pWorker,20))
   {
      if (l_pWorker)
      {
         printf("ServerNode::m_WorkQueue\n");
         delete l_pWorker;
         l_pWorker = NULL;
      }
   }
   
   while (m_RxQueue.pop(l_pWorker,20))
   {
      if (l_pWorker)
      {
         printf("ServerNode::m_RxQueue\n");
         delete l_pWorker;
         l_pWorker = NULL;
      }
   }
   
   while (m_TxQueue.pop(l_pWorker,20))
   {
      if (l_pWorker)
      {
         printf("ServerNode::m_TxQueue\n");
         delete l_pWorker;
         l_pWorker = NULL;
      }
   }
}
