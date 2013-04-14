#ifndef  UPDATE_REQUEST_PACKET_H
#define  UPDATE_REQUEST_PACKET_H

#include "ChatPacket.h"

struct __attribute__((__packed__)) UpdateRequestData
{
   ui32  user_id;
   ui32  timestamp;
};

class UpdateRequestPacket : public ChatPacket
{
public:
   
   UpdateRequestPacket();
   
   UpdateRequestPacket(ui32 userId, ui32 lastTs);
   
   bool  setId(ui32 userId);
   
   bool  getId(ui32 &userId) const;
   
   bool  setTs(ui32 lastTs);
   
   bool  getTs(ui32 &lastTs) const;
   
private:
   
};

#endif   // UPDATE_REQUEST_PACKET_H