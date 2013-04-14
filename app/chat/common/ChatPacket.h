#ifndef  CHAT_PACKET_H
#define  CHAT_PACKET_H

#include "GenericPacket.h"

struct __attribute__((__packed__)) ChatPacketHdr
{
   static const ui32  marker   = 0xA5A5A5A5;
   ui32  type;
   ui32  length;
};

class ChatPacket : public GenericPacket
{
public:
   
   enum ChatPacketType
   {
      TypeNotSet,
      ChatMsgType,
      UpdateRequestType,
      UpdateResponseType
   };
   
   ChatPacket();
   
   ChatPacket(ChatPacketType type, ui32 nDataSize);
   
   ChatPacketType type() const;
      
   //virtual bool  pack(void** pData, unsigned int &nSizeBytes) const;
   
   virtual bool  unpack(const void* pPkt, unsigned int nSizeBytes);
      
private:
   
};

#endif   // CHAT_PACKET_H