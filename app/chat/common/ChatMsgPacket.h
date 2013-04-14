#ifndef  CHAT_MSG_PACKET_H
#define  CHAT_MSG_PACKET_H

#include "ChatPacket.h"
#include "ChatMsg.h"

class ChatMsgPacket : public ChatPacket
{
public:
   
   ChatMsgPacket(const ChatMsg &msg);
   
   bool  getMsg(ChatMsg &msg) const;
   
   //bool  pack(void** pData, unsigned int &nSizeBytes) const;
   
   //bool  unpack(void* pPkt, unsigned int nSizeBytes);
   
private:
   
};

#endif   // CHAT_PACKET_H