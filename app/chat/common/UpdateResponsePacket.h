#ifndef  UPDATE_RESPONSE_PACKET_H
#define  UPDATE_RESPONSE_PACKET_H

#include <vector>
#include "ChatPacket.h"
#include "ChatMsg.h"

class UpdateResponsePacket : public ChatPacket
{
public:
   
   UpdateResponsePacket();
   
   UpdateResponsePacket(ui32 ts, const std::vector<ChatMsg*> &msgs);

   bool  getMsgList(std::vector<ChatMsg*> &msgs) const;
   
   bool  setTs(ui32 lastTs);
   
   bool  getTs(ui32 &lastTs) const;
   
private:
   
};

#endif   // UPDATE_RESPONSE_PACKET_H