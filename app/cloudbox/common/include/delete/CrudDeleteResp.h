#ifndef CB_DELETE_MSG_H
#define CB_DELETE_MSG_H

#include "DeleteMsg.h"

class CbDeleteMsg : public DeleteMsg
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbDeleteMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef DeleteMsg inherited;

};

#endif // CB_DELETE_MSG_H
