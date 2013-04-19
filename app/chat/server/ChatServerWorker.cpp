#include <string.h>
#include <stdio.h>
#include "ChatMsgPacket.h"
#include "UpdateRequestPacket.h"
#include "UpdateResponsePacket.h"
#include "ChatServerWorker.h"

//------------------------------------------------------------------------------
ChatServerWorker::ChatServerWorker(TcpSocket* pSocket)
 : ServerWorker(pSocket)
{
   m_pChatLog = NULL;
}

//------------------------------------------------------------------------------
ChatServerWorker::~ChatServerWorker()
{
   
}

//------------------------------------------------------------------------------
void ChatServerWorker::setChatLog(ChatLog *pLog)
{
   m_pChatLog = pLog;
}

//------------------------------------------------------------------------------
bool ChatServerWorker::processMsg(const char* pMsg, ui32 nMsgLenBytes)
{
   bool        l_bSuccess = false;
   ChatPacket* l_pPacket = NULL;
   
   printf("ChatServerWorker::processMsg: bytes = %u\n", nMsgLenBytes);
   
   l_pPacket = new ChatPacket();
   if (l_pPacket == NULL)
   {
      printf("ChatServerWorker::processMsg: Failed to allocate packet\n");
   }
   
   l_bSuccess = l_pPacket->unpack(pMsg, nMsgLenBytes);
   if (!l_bSuccess)
   {
      return false;
   }
   
   switch (l_pPacket->type())
   {
      case ChatPacket::ChatMsgType:
      case ChatPacket::UpdateRequestType:
         pushRx(l_pPacket);
         break;
            
      default:
         printf("ChatServerWorker::processMsg: Unrecognized packet\n");
         break;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool ChatServerWorker::work()
{
   ChatPacket* l_pPacket = NULL;
   ui32        l_nCurrentTs = 0;
   UpdateResponsePacket* l_pUpdateResp = NULL;
   std::vector<ChatMsg*> l_vChatMsgVec;
   
   // Attempt to receive a packet from the queue.
   if (!popRx(l_pPacket))
   {
      return false;
   }
   
   switch (l_pPacket->type())
   {
      case ChatPacket::ChatMsgType:
         {
            ChatMsg msg;
            ChatMsgPacket* l_pChatMsgPkt = NULL;
            l_pChatMsgPkt = reinterpret_cast<ChatMsgPacket*>(l_pPacket);
            
            l_pChatMsgPkt->getMsg(msg);
            l_nCurrentTs = m_pChatLog->append(msg);
            
            l_pUpdateResp = new UpdateResponsePacket(l_nCurrentTs,
                                                     l_vChatMsgVec);
            //l_pUpdateResp->setTs(l_nCurrentTs);
            pushTx(l_pUpdateResp);
         }
         break;
         
      case ChatPacket::UpdateRequestType:
         {
            ui32 userId = 0;
            ui32 lastTs = 0;
            
            //std::vector<ChatMsg> l_vChatMsgVec;
            UpdateRequestPacket* l_pUpdateReqPkt = NULL;
            l_pUpdateReqPkt = reinterpret_cast<UpdateRequestPacket*>(l_pPacket);
            
            l_pUpdateReqPkt->getId(userId);
            l_pUpdateReqPkt->getTs(lastTs);
            l_nCurrentTs = m_pChatLog->read(userId, lastTs, l_vChatMsgVec);
            
            l_pUpdateResp = new UpdateResponsePacket(l_nCurrentTs,
                                                     l_vChatMsgVec);
            pushTx(l_pUpdateResp);
         }
         break;
         
      default:
         printf("ChatServerWorker::processMsg: Unrecognized packet\n");
         break;
   }
   
   return true;
}


//------------------------------------------------------------------------------
bool ChatServerWorker::getMsg(char** pMsg, ui32 &nMsgLenBytes)
{
   bool        l_bSuccess = false;
   ChatPacket* l_pChatPkt = NULL;
   
   l_bSuccess = popTx(l_pChatPkt);
   if (!l_bSuccess)
   {
      return false;
   }
   
   l_bSuccess = l_pChatPkt->pack((void**)pMsg, nMsgLenBytes);
   
   delete l_pChatPkt;
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
ui32 ChatServerWorker::headerSize() const
{
   return sizeof(ChatPacketHdr);
}

//------------------------------------------------------------------------------
ui32 ChatServerWorker::getExpPayloadSize(const char* pHeader) const
{
   const ChatPacketHdr* l_pPktHdr = NULL;
   
   l_pPktHdr = reinterpret_cast<const ChatPacketHdr*>(pHeader);
   
   if (l_pPktHdr == NULL) return 0;
   
   if (l_pPktHdr->marker != ChatPacketHdr::marker) return 0;
   
   return l_pPktHdr->length;
}

