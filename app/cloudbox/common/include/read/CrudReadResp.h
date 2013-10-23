#ifndef CB_READ_MSG_H
#define CB_READ_MSG_H

#include "ReadMsg.h"

class CbReadMsg : public ReadMsg
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbReadMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef ReadMsg inherited;

};

#endif // CB_READ_MSG_H
