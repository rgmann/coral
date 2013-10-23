#ifndef CB_UPDATE_FILE_MSG_H
#define CB_UPDATE_FILE_MSG_H

#include "GenericPacket.h"

class CbUpdateFileMsg : public GenericPacket
{
public:

   enum Type
   {
      // TODO: add types
   }

   struct __attribute ((__packed__)) Data
   {
   };

   explicit CbUpdateFileMsg();

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // CB_UPDATE_FILE_MSG_H
