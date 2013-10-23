#ifndef CB_CREATE_FILE_MSG_H
#ifndef CB_CREATE_FILE_MSG_H

#include "GenericPacket.h"

class CbCreateFileMsg : public GenericPacket
{
public:

   enum Type
   {
   }

   struct __attribute__ ((__packed__)) Data
   {
      // filepath
   };

   CbCreateFileMsg();
   
   CbCreateFileMsg(const std::string &path);
   
   bool  unpack(void* pPkt, ui32 nSizeBytes);
   
private:
   
   typedef GenericPacket inherited;
};

#endif // CB_CREATE_FILE_MSG_H
