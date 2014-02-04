#ifndef CB_READ_FILE_ACK_MSG_H
#define CB_READ_FILE_ACK_MSG_H

#include "GenericPacket.h"

class CbReadFileAckMsg : public GenericPacket
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbReadFileAckMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // CB_READ_FILE_ACK_MSG_H