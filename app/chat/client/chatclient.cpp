#include <stdio.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include "ArgParser.h"
#include "TcpSocket.h"
#include "TcpClient.h"
#include "Thread.h"
#include "Queue.h"
#include "ChatMsg.h"
#include "ChatMsgPacket.h"
#include "UpdateRequestPacket.h"
#include "UpdateResponsePacket.h"

bool  g_bStopCommanded = false;
Queue<ChatMsg> g_ChatMstQ;

std::string g_sHostName;
int         g_nPort = 5000;
int         g_nUserId = 0;

void sighandler(int s)
{
   g_bStopCommanded = true;
}

void printChatMsg(const ChatMsg &msg);
void rxThread(ThreadArg* pArg);
void inputThread(ThreadArg* pArg);

int main(int argc, char *argv[])
{
   ArgParser args;
   Thread*  m_pRxThread = NULL;
   Thread*  m_pInputThread = NULL;
      
   printf("Chat Client:\n\n");
   
   args.addArg("name: HostName, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");
   
   args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");
   
   args.addArg("name: UID, primary: u, alt: uid, type: opt, \
               vtype: int, required, desc: User ID");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
   args.getArgVal(g_sHostName, Argument::ArgName, "HostName");
   args.getArgVal(g_nPort, Argument::ArgName, "Port");
   args.getArgVal(g_nUserId, Argument::ArgName, "UID");
   
   m_pRxThread = Thread::Create(rxThread, NULL);
   
   if (m_pRxThread == NULL)
   {
      printf("Failed to start RX thread.\n");
      return 1;
   }
   
   m_pInputThread = Thread::Create(inputThread, NULL);
   
   if (m_pInputThread == NULL)
   {
      printf("Failed to start input thread.\n");
      return 1;
   }
   
   while (!g_bStopCommanded)
   {
      sleep(1);
   }
   
   if (m_pRxThread)
   {
      m_pRxThread->stop();
      m_pRxThread->join();
   }
   
   return 0;
}

void printChatMsg(const ChatMsg &msg)
{
   char msgStr[170];
   ui32 l_nMsgLen = 0;
   ui32 l_nUserId = 0;
   
   msg.getMsg(msgStr, l_nMsgLen);
   l_nUserId = msg.getUserId();
   
   printf("%d: %s\n", l_nUserId, msgStr);
}

//------------------------------------------------------------------------------
void rxThread(ThreadArg* pArg)
{
   TcpSocket*  l_pSocket = NULL;
   
   ChatPacketHdr  header;
   ui32           l_nDataLen     = 0;
   ui32           l_nCurrentTs = 0;
   ui32           l_nBytesRecvd = 0;
   
   ChatMsg  msg;
   
   std::vector<ChatMsg*> msgVecRx;
   std::vector<ChatMsg*>::iterator msgVecRxIt;
   
   UpdateRequestPacket* l_pUpdateReq = NULL;
   char*       l_pPackedUpdateReq    = NULL;
   ui32        l_nPackedUpdateReqLen = 0;
   
   char*       l_pPkt = NULL;
   
   ChatPacket* l_pChatPkt = NULL;
   
   // Connect to server
   l_pSocket = TcpClient::Connect(g_sHostName.c_str(), g_nPort, 2000);
   
   if (l_pSocket == NULL)
   {
      printf("Failed to connect to server\n");
      while (!pArg->stopSignalled()) sleep(1);
   }
   
   while (!pArg->stopSignalled())
   {
      l_pPkt = NULL;
      l_pChatPkt = NULL;
      
      l_pUpdateReq = new UpdateRequestPacket(g_nUserId, l_nCurrentTs);
      if (l_pUpdateReq)
      {
         l_pUpdateReq->pack((void**)&l_pPackedUpdateReq,
                            l_nPackedUpdateReqLen);
         l_pSocket->send(l_pPackedUpdateReq, l_nPackedUpdateReqLen);
      }
      
      l_nBytesRecvd = l_pSocket->recv((char*)&header,
                                      sizeof(ChatPacketHdr),
                                      1000);
      
      if (header.marker != ChatPacketHdr::marker)
      {
         continue;
      }

      l_nDataLen = header.length;
      l_pPkt = new char[sizeof(ChatPacketHdr) + l_nDataLen];
      if (!l_pPkt) continue;
      
      memcpy(l_pPkt, &header, sizeof(ChatPacketHdr));
      
      l_nBytesRecvd = l_pSocket->recv(l_pPkt + sizeof(ChatPacketHdr),
                                       l_nDataLen, 1000);
      
      if (l_nBytesRecvd != l_nDataLen)
      {
         delete l_pPkt;
         continue;
      }
      
      if (header.type != ChatPacket::UpdateResponseType)
      {
         delete l_pPkt;
         continue;
      }
      
      UpdateResponsePacket* l_pRespPkt = new UpdateResponsePacket();
      
      l_pRespPkt->unpack(l_pPkt, sizeof(ChatPacketHdr) + l_nDataLen);
      
      l_pRespPkt->getMsgList(msgVecRx);
      l_pRespPkt->getTs(l_nCurrentTs);
      
      msgVecRxIt = msgVecRx.begin();
      for (; msgVecRxIt < msgVecRx.end(); ++msgVecRxIt)
      {
         printChatMsg(*(*msgVecRxIt));
      }
      
      // Check for a new msg in the tx queue
      if (g_ChatMstQ.pop(msg, 0))
      {
         ChatMsgPacket* pMsgPacket = NULL;
         char*          pMsgData = NULL;
         ui32           msgLen = 0;
         
         pMsgPacket = new ChatMsgPacket(msg);
         pMsgPacket->pack((void**)&pMsgData, msgLen);
         
         l_pSocket->send(pMsgData, msgLen);
      }
   }
}

//------------------------------------------------------------------------------
void inputThread(ThreadArg* pArg)
{
   std::string msgString;
   ChatMsg  msg;
   
   while (!pArg->stopSignalled())
   {
      std::cin >> msgString;
      
      msg.setUserId(g_nUserId);
      msg.setMsg(msgString.c_str(), msgString.length());
   }
}
