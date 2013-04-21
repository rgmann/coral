#include <string.h>
#include <stdio.h>
#include "ChatMsg.h"

//------------------------------------------------------------------------------
ChatMsg::ChatMsg()
{
   reset();
}

//------------------------------------------------------------------------------
ChatMsg::ChatMsg(const ChatMsg &other)
{
   m_nUserId = other.m_nUserId;
   m_nTs = other.m_nTs;
   memcpy(m_sMsg, other.m_sMsg, MaxMsgLen);
}

//------------------------------------------------------------------------------
void ChatMsg::reset()
{
   m_nUserId = 0;
   m_nTs     = 0;
   memset(m_sMsg, 0, MaxMsgLen);
}

//------------------------------------------------------------------------------
void  ChatMsg::setUserId(ui32 userId) { m_nUserId = userId; }

//------------------------------------------------------------------------------
ui32  ChatMsg::getUserId() const { return m_nUserId; }

//------------------------------------------------------------------------------
void  ChatMsg::setMsg(const char* pMsg, ui32 nMsgLen)
{
   //printf("ChatMsg::setMsg: str = %s, len = %u\n", pMsg, nMsgLen);
   strncpy(m_sMsg, pMsg, (nMsgLen < MaxMsgLen) ? nMsgLen : MaxMsgLen);
}

//------------------------------------------------------------------------------
void ChatMsg::getMsg(char* pMsg, ui32 &nMsgLen) const
{
   memset(pMsg, 0, MaxMsgLen);
   strncpy(pMsg, m_sMsg, strlen(m_sMsg));
   nMsgLen = strlen(m_sMsg);
}

//------------------------------------------------------------------------------
void  ChatMsg::setTs(ui32 ts) { m_nTs = ts; }

//------------------------------------------------------------------------------
ui32  ChatMsg::getTs() const { return m_nTs; }

//------------------------------------------------------------------------------
void ChatMsg::pack(ChatMsgData* pMsg) const
{
   if (!pMsg) return;
   
//   printf("ChatMsg::pack: str = %s, uid = %u, ts=%u\n",
//          m_sMsg, m_nUserId, m_nTs);
   pMsg->user_id = m_nUserId;
   pMsg->timestamp = m_nTs;
   pMsg->msg_len = 0; // Messages are always the same size for now.
   strncpy(pMsg->msg, m_sMsg, MaxMsgLen);
}

//------------------------------------------------------------------------------
void ChatMsg::unpack(const ChatMsgData* pMsg)
{
   if (!pMsg) return;
   
//   printf("ChatMsg::unpack: str = %s, uid = %u, ts=%u\n",
//          pMsg->msg, pMsg->user_id, pMsg->timestamp);
   m_nUserId = pMsg->user_id;
   m_nTs = pMsg->timestamp;
   strncpy(m_sMsg, pMsg->msg, MaxMsgLen);
}
