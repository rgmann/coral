#ifndef CB_DELETE_FILE_MSG_H
#define CB_DELETE_FILE_MSG_H

#include "GenericPacket.h"

class CbDeleteFileMsg : public GenericPacket
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbDeleteFileMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // CB_DELETE_FILE_MSG_H
