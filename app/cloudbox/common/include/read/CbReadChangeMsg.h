#ifndef CB_READ_CHANGE_MSG_H
#define CB_READ_CHANGE_MSG_H

#include "GenericPacket.h"

class CbReadChangeMsg : public GenericPacket
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbReadChangeMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // CB_READ_CHANGE_MSG_H
