#include <cstddef>
#include "ChatMsgPacket.h"

//------------------------------------------------------------------------------
ChatMsgPacket::ChatMsgPacket(const ChatMsg &msg)
: ChatPacket(ChatMsgType, sizeof(ChatMsgData))
{
   if (dataPtr())
   {
      ChatMsgData* l_pData = reinterpret_cast<ChatMsgData*>(dataPtr());
      msg.pack(l_pData);
   }
}

//------------------------------------------------------------------------------
bool ChatMsgPacket::getMsg(ChatMsg &msg) const
{
   const ChatMsgData* l_pData = reinterpret_cast<const ChatMsgData*>(dataPtr());
   
   if (l_pData == NULL)
   {
      return false;
   }
   
   msg.unpack(l_pData);
   
   return true;
}