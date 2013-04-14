#include <string.h>
#include <stdio.h>
#include "UpdateResponsePacket.h"

//------------------------------------------------------------------------------
UpdateResponsePacket::UpdateResponsePacket()
{
}

//------------------------------------------------------------------------------
UpdateResponsePacket::UpdateResponsePacket(ui32 ts,
                                           const std::vector<ChatMsg*> &msgs)
: ChatPacket(UpdateResponseType,
             2 * sizeof(ui32) + sizeof(ChatMsgData) * msgs.size())
{
   std::vector<ChatMsg*>::const_iterator l_iMsgIt;
   ui32 l_nOffs = 0;
   ui32  l_nMsgCnt = 0;
   char* l_pData = reinterpret_cast<char*>(dataPtr());
   
   if (l_pData)
   {
      // First four bytes of UpdateResponsePacket data is the timestamp
      memcpy(l_pData, &ts, sizeof(ui32));
      l_nOffs += sizeof(ui32);
      
      l_nMsgCnt = msgs.size();
      memcpy((l_pData + l_nOffs), &l_nMsgCnt, sizeof(ui32));
      l_nOffs += sizeof(ui32);
      
      l_iMsgIt = msgs.begin();
      for (; l_iMsgIt < msgs.end(); ++l_iMsgIt)
      {
         ChatMsgData* l_pMsgData = (ChatMsgData*)(l_pData + l_nOffs);
         
         (*l_iMsgIt)->pack(l_pMsgData);
         
         l_nOffs += sizeof(ChatMsgData);
      }
   }
}

//------------------------------------------------------------------------------
bool UpdateResponsePacket::getMsgList(std::vector<ChatMsg*> &msgs) const
{
   ui32  l_nOffs = 0;
   char* l_pData = reinterpret_cast<char*>(dataPtr());
   
   if (l_pData == NULL)
   {
      return false;
   }
   
   // Skip over the start of list timestamp field
   l_nOffs += sizeof(ui32);
   l_nOffs += sizeof(ui32);
   
   while (l_nOffs < dataSize())
   {
      ChatMsg* l_pChatMsg = new ChatMsg();
      const ChatMsgData* l_pMsgData = NULL;
      
      l_pMsgData = reinterpret_cast<ChatMsgData*>(l_pData + l_nOffs);
      l_pChatMsg->unpack(l_pMsgData);
      
      msgs.push_back(l_pChatMsg);
      
      l_nOffs += sizeof(ChatMsgData);
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool UpdateResponsePacket::setTs(ui32 lastTs)
{
   ui32* pTs = reinterpret_cast<ui32*>(dataPtr());
   
   if (pTs == NULL)
   {
      return false;
   }
   
   *pTs = lastTs;
   
   return true;
}

//------------------------------------------------------------------------------
bool UpdateResponsePacket::getTs(ui32 &lastTs) const
{
   ui32* pTs = reinterpret_cast<ui32*>(dataPtr());
   
   if (pTs == NULL)
   {
      return false;
   }
   
   lastTs = *pTs;
   
   return true;
}
