#ifndef CB_UPDATE_MSG_H
#define CB_UPDATE_MSG_H

#include "UpdateMsg.h"

class CbUpdateMsg : public UpdateMsg
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbUpdateMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef UpdateMsg inherited;

};

#endif // CB_UPDATE_MSG_H
