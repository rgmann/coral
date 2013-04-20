#include <stdio.h>
#include <string.h>
#include "ServerNode.h"

//------------------------------------------------------------------------------
ServerNode::ServerNode()
{
   m_pRxThread = NULL;
   m_pWorkerThread = NULL;
   m_pTxThread = NULL;
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
   //m_vWorkerList.push_back(pWorker);
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
   if (m_pRxThread)
   {
      m_pRxThread->stop();
      m_pRxThread->join();
   }
   
   if (m_pWorkerThread)
   {
      m_pRxThread->stop();
      m_pRxThread->join();
   }
   
   if (m_pTxThread)
   {
      m_pRxThread->stop();
      m_pRxThread->join();
   }
   
   return true;
}

//------------------------------------------------------------------------------
void ServerNode::rxThreadFunc(ThreadArg* pArg)
{
   ((ServerNode*)pArg->pUserData)->rxThread(pArg);
}

//------------------------------------------------------------------------------
void ServerNode::rxThread(ThreadArg* pArg)
{
   TcpSocket*  l_pSocket = NULL;
   
   ui32         l_nHeaderLen   = 0;
   ui32         l_nDataLen     = 0;
   ui32         l_nPktLen      = 0;
   ui32         l_nBytesRecvd  = 0;
   
   char* l_pHeader = NULL;
   char* l_pPkt    = NULL;
   
   ServerWorker*  l_pWorker = NULL;
   
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
         
      l_pSocket = l_pWorker->socket(500);
      if (l_pSocket == NULL)
      {
         continue;
      }
         
      // Each worker has a standard application specific header.  Each
      // packet should be preceded by a header and the header should
      // indicate the length of the following data.
      l_nHeaderLen   = l_pWorker->headerSize();
      
      l_pHeader = new char[l_nHeaderLen];
      
      l_nBytesRecvd  = l_pSocket->recv(l_pHeader, l_nHeaderLen, 20);
      printf("ServerNode::rxThread: exp = %u, recvd = %u\n",
             l_nHeaderLen, l_nBytesRecvd);
         
      if (l_nHeaderLen != l_nBytesRecvd)
      {
         delete[] l_pHeader;
         l_pWorker->releaseSocket();
            
         continue;
      }
         
      // Pass the header to the worker so that it can parse the
      // expected payload length.
      l_nDataLen = l_pWorker->getExpPayloadSize(l_pHeader);
         
      if (l_nDataLen > 0)
      {
         l_nPktLen = l_nHeaderLen + l_nDataLen;
         l_pPkt = new char[l_nPktLen];
            
         // Copy the header into the full packet
         memcpy(l_pPkt, l_pHeader, l_nHeaderLen);
            
         l_nBytesRecvd  = l_pSocket->recv(l_pPkt + l_nHeaderLen,
                                          l_nDataLen, 0);
            
         if (l_nBytesRecvd == l_nDataLen)
         {
            l_pWorker->processMsg(l_pPkt, l_nPktLen);
         }
            
         delete[] l_pPkt;
         l_pPkt = NULL;
      }
         
      // Delete the header
      delete[] l_pHeader;
      l_pHeader = NULL;
         
      // Release the socket so that it can be used by the TX thread.
      l_pWorker->releaseSocket();
      
      // Push the worker into the work queue
      m_WorkQueue.push(l_pWorker, 500);
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
         
      // Get the worker's socket
      if (l_pWorker == NULL)
      {
         continue;
      }
         
      l_pSocket = l_pWorker->socket(500);
      if (l_pSocket == NULL)
      {
         continue;
      }
         
      // Each worker has a standard application specific header.  Each
      // packet should be preceded by a header and the header should
      // indicate the length of the following data.
      l_bMsgAvailable = l_pWorker->getMsg(&l_pMsg, l_nMsgLen);
      if (!l_bMsgAvailable)
      {
         continue;
      }
         
      // Send the message
      l_pSocket->send(l_pMsg, l_nMsgLen);
      
      // Release the socket so that it can be used by the TX thread.
      l_pWorker->releaseSocket();

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
