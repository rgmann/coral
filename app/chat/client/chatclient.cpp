#include <stdio.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include "ArgParser.h"
#include "TcpClient.h"
#include "ChatMsg.h"
#include "ChatMsgPacket.h"
#include "UpdateRequestPacket.h"
#include "UpdateResponsePacket.h"

bool  g_bStopCommanded = false;

void sighandler(int s)
{
   g_bStopCommanded = true;
}

int main(int argc, char *argv[])
{
   ArgParser args;
   TcpSocket*  l_pSocket = NULL;
   
   std::string l_sHostName;
   int         l_nPort = 5000;
   
   char buffer[256];
   int l_nBytes = 0;
   
   printf("Client test\r\n");
   
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
   
   args.getArgVal(l_sHostName, Argument::ArgName, "HostName");
   args.getArgVal(l_nPort, Argument::ArgName, "Port");
   
   locpSocket = TcpClient::Connect(l_sHostName.c_str(), l_nPort, 2000);
   
   while (!g_bStopCommanded)
   {
      
   }
   
   return 0;
}

void printChatPktHdr(const ChatPacketHdr* pHdr)
{
   printf("Hdr: marker = %u\n", pHdr->marker);
   printf("Hdr: type = %u\n", pHdr->type);
   printf("Hdr: length = %u\n", pHdr->length);
}

void testChatMessagePkt()
{
   ChatMsg   msg;
   ChatMsgPacket* pMsgPacket = NULL;
   ChatMsgData* pChatMsgData = NULL;
   
   char*          pMsgData = NULL;
   ui32           msgLen = 0;
   
   char  msgStr[] = "Test message";
   
   printf("testChatMessagePkt:\n");
   
   msg.setMsg(msgStr, strlen(msgStr));
   msg.setTs(15);
   msg.setUserId(13631);
   
   pMsgPacket = new ChatMsgPacket(msg);
   pMsgPacket->pack((void**)&pMsgData, msgLen);
   printf("pMsgPacket->pack: msgLen = %u\n", msgLen);
   
   ChatPacketHdr* pHdr = reinterpret_cast<ChatPacketHdr*>(pMsgData);
   
   printChatPktHdr(pHdr);
   
   pChatMsgData = reinterpret_cast<ChatMsgData*>(pMsgData +
                                                sizeof(ChatPacketHdr));
   
   printf("Msg: user_id = %u\n", pChatMsgData->user_id);
   printf("Msg: timestamp = %u\n", pChatMsgData->timestamp);
   printf("Msg: msg_len = %u\n", pChatMsgData->msg_len);
   printf("Msg: msg = %s\n", pChatMsgData->msg);
   
   printf("\n");
   
   delete pMsgPacket;
   delete[] pMsgData;
}

void testUpdateRequestPkt()
{
   ui32  userId = 13631;
   ui32  userIdRx = 0;
   ui32  lastTs = 15;
   ui32  lastTsRx = 0;
   
   UpdateRequestPacket* pReqTx = NULL;
   UpdateRequestPacket* pReqRx = NULL;
   UpdateRequestData*   pUpdReqData = NULL;
   
   char*          pUpdateReq = NULL;
   ui32           msgLen = 0;
   
   printf("testUpdateRequestPkt:\n");
   
   pReqTx = new UpdateRequestPacket(userId, lastTs);
   pReqRx = new UpdateRequestPacket();
   
   pReqTx->pack((void**)&pUpdateReq, msgLen);
   
   ChatPacketHdr* pHdr = reinterpret_cast<ChatPacketHdr*>(pUpdateReq);
   
   printChatPktHdr(pHdr);
   
   pUpdReqData = reinterpret_cast<UpdateRequestData*>(pUpdateReq +
                                                 sizeof(ChatPacketHdr));
   
   printf("Req: user_id = %u\n", pUpdReqData->user_id);
   printf("Req: timestamp = %u\n", pUpdReqData->timestamp);
   
   
   pReqRx->unpack(pUpdateReq, msgLen);
   
   pReqRx->getId(userIdRx);
   printf("pReqRx: user_id = %u\n", userIdRx);
   
   pReqRx->getTs(lastTsRx);
   printf("pReqRx: timestamp = %u\n", lastTsRx);
   
   printf("\n");
   
   delete pReqRx;
   delete pReqTx;
   delete[] pUpdateReq;
}

void testUpdateResponsePkt()
{
   std::vector<ChatMsg*> msgVecTx;
   
   std::vector<ChatMsg*> msgVecRx;
   std::vector<ChatMsg*>::iterator msgVecRxIt;
   
   UpdateResponsePacket* pUpdRespTx = NULL;
   UpdateResponsePacket* pUpdRespRx = NULL;

   ui32  nBaseId = 12631;
   ui32  nBaseTs = 15;
   ui32  nNextTs = nBaseTs;
   
   char*          pUpdRespData = NULL;
   ui32           msgLen = 0;
   
   printf("testUpdateResponsePkt:\n");
   
   while (msgVecTx.size() < 5)
   {
      ChatMsg* pMsg = NULL;
      char  msgStr[170];
      
      pMsg = new ChatMsg();
      
      snprintf(msgStr, 170, "Test string #%d", (int)msgVecTx.size());
      
      pMsg->setUserId(nBaseId++);
      pMsg->setTs(nNextTs++);
      pMsg->setMsg(msgStr, strlen(msgStr));
      
      msgVecTx.push_back(pMsg);
   }

   pUpdRespTx = new UpdateResponsePacket(nBaseTs, msgVecTx);
   pUpdRespTx->pack((void**)&pUpdRespData, msgLen);
   
   pUpdRespRx = new UpdateResponsePacket();
   pUpdRespRx->unpack(pUpdRespData, msgLen);
   pUpdRespRx->getMsgList(msgVecRx);
   
   pUpdRespRx->getTs(nNextTs);
   printf("Msg: ts = %u\n", nNextTs);
   
   msgVecRxIt = msgVecRx.begin();
   for (; msgVecRxIt < msgVecRx.end(); ++msgVecRxIt)
   {
      char  msgStr[170];
      ui32  msgLen = 0;
      
      printf("Msg: ts = %u\n", (*msgVecRxIt)->getTs());
      printf("Msg: userId = %u\n", (*msgVecRxIt)->getUserId());
      (*msgVecRxIt)->getMsg(msgStr, msgLen);
      printf("Msg: len = %u, msg = %s\n", msgLen, msgStr);
   }
   
   printf("\n");
   
   delete pUpdRespTx;
   delete pUpdRespRx;
   delete[] pUpdRespData;
}
