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
   printf("Got signal\n");
   g_bStopCommanded = true;
}

void printChatMsg(const ChatMsg &msg);
void rxThread(ThreadArg* pArg);
void inputThread(ThreadArg* pArg);
i32  receiveRespUpdate(UpdateResponsePacket** pResp, TcpSocket* pSocket);

int main(int argc, char *argv[])
{
   ArgParser args;
   Thread*  m_pRxThread = NULL;
   Thread*  m_pInputThread = NULL;
   
   struct sigaction sigIntHandler;
      
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
   
   sigIntHandler.sa_handler = sighandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;
   sigaction(SIGINT, &sigIntHandler, NULL);
   
   g_ChatMstQ.initialize(20);
   
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
   
   printf("Stopping\n");
   
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
   
   printf("%d, %d: %s\n", l_nUserId, msg.getTs(), msgStr);
}

//------------------------------------------------------------------------------
void rxThread(ThreadArg* pArg)
{
   TcpSocket*  l_pSocket = NULL;
   ui32           l_nCurrentTs = 0;
   
   
   ChatMsg  msg;
   
   std::vector<ChatMsg*> msgVecRx;
   std::vector<ChatMsg*>::iterator msgVecRxIt;
   
   UpdateRequestPacket* l_pUpdateReq = NULL;
   UpdateResponsePacket* l_pUpdResp = NULL;
   
   
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
      l_pChatPkt = NULL;
      
      // Check for a new msg in the tx queue
      if (g_ChatMstQ.pop(msg, 1000))
      {
         ChatMsgPacket* pMsgPacket = NULL;
         char*          pMsgData = NULL;
         ui32           msgLen = 0;
         
         pMsgPacket = new ChatMsgPacket(msg);
         pMsgPacket->pack((void**)&pMsgData, msgLen);
         
         l_pSocket->send(pMsgData, msgLen);
         
         delete[] pMsgData;
         pMsgData = NULL;
         
         if (receiveRespUpdate(&l_pUpdResp, l_pSocket) == -1)
         {
            printf("Socket closed by master\n");
            g_bStopCommanded = true;
            break;
         }
         
         if (l_pUpdResp)
         {
            ui32 l_nNewTs = 0;
            l_pUpdResp->getMsgList(msgVecRx);
            l_pUpdResp->getTs(l_nNewTs);
            
            if (l_nNewTs > l_nCurrentTs)
            {
               l_nCurrentTs = l_nNewTs;
               
               msgVecRxIt = msgVecRx.begin();
               for (; msgVecRxIt < msgVecRx.end(); ++msgVecRxIt)
               {
                  printChatMsg(*(*msgVecRxIt));
               }
               
               // Done't need to store the messages.
               msgVecRx.clear();
            }
            
            delete l_pUpdResp;
            l_pUpdResp = NULL;
         }
      }
      
      // Request that the server send all chat messages after the specified
      // logical timestamp.
      l_pUpdateReq = new UpdateRequestPacket(g_nUserId, l_nCurrentTs);
      if (l_pUpdateReq)
      {
         char*       l_pPackedUpdateReq    = NULL;
         ui32        l_nPackedUpdateReqLen = 0;
                  
         l_pUpdateReq->pack((void**)&l_pPackedUpdateReq,
                            l_nPackedUpdateReqLen);
         l_pSocket->send(l_pPackedUpdateReq, l_nPackedUpdateReqLen);
         
         delete[] l_pPackedUpdateReq;
         l_pPackedUpdateReq = NULL;
         
         if (receiveRespUpdate(&l_pUpdResp, l_pSocket) == -1)
         {
            printf("Socket closed by master\n");
            g_bStopCommanded = true;
            break;
         }
         
         if (l_pUpdResp)
         {
            ui32 l_nNewTs = 0;
            l_pUpdResp->getMsgList(msgVecRx);
            l_pUpdResp->getTs(l_nNewTs);
            
            if (l_nNewTs > l_nCurrentTs)
            {
               l_nCurrentTs = l_nNewTs;
               
               msgVecRxIt = msgVecRx.begin();
               for (; msgVecRxIt < msgVecRx.end(); ++msgVecRxIt)
               {
                  printChatMsg(*(*msgVecRxIt));
               }
               
               // Done't need to store the messages.
               msgVecRx.clear();
            }
            
            delete l_pUpdResp;
            l_pUpdResp = NULL;
         }
      }
   }
   
   l_pSocket->closeSocket();
}

//------------------------------------------------------------------------------
void inputThread(ThreadArg* pArg)
{
   std::string msgString;
   ChatMsg  msg;
   
   while (!pArg->stopSignalled())
   {
      msgString = "";
      std::getline(std::cin, msgString);
      
      msg.reset();
      msg.setUserId(g_nUserId);
      msg.setMsg(msgString.c_str(), msgString.length());
      
      g_ChatMstQ.push(msg, 0);
   }
}

//------------------------------------------------------------------------------
i32 receiveRespUpdate(UpdateResponsePacket** pResp, TcpSocket* pSocket)
{
   ChatPacketHdr  header;
   bool           l_bSuccess = false;
   i32           l_nBytesRecvd = 0;
   char*          l_pPkt = NULL;
   ui32           l_nDataLen     = 0;

   
   // Attempt to receive a chat packet header
   l_nBytesRecvd = pSocket->recv((char*)&header,
                                 sizeof(ChatPacketHdr),
                                 1000);
   
   if (l_nBytesRecvd == -1)
   {
      return -1;
   }
   
   if (header.marker != ChatPacketHdr::marker) return false;
   
   // Determine how much data is included in the packet.
   l_nDataLen = header.length;
   l_pPkt = new char[sizeof(ChatPacketHdr) + l_nDataLen];
   
   if (!l_pPkt) return false;
   
   // Copy the previously read header into the allocated packet.
   memcpy(l_pPkt, &header, sizeof(ChatPacketHdr));
   
   // Receive the rest of the packet directly into the allocated buffer.
   l_nBytesRecvd = pSocket->recv(l_pPkt + sizeof(ChatPacketHdr),
                                 l_nDataLen, 1000);
   
   if ((l_nBytesRecvd == l_nDataLen) &&
       (header.type == ChatPacket::UpdateResponseType))
   {
      *pResp = new UpdateResponsePacket();
      (*pResp)->unpack(l_pPkt, sizeof(ChatPacketHdr) + l_nDataLen);
   }
   
   if (l_pPkt)
   {
      delete l_pPkt;
   }
   
   return l_nBytesRecvd;
}
