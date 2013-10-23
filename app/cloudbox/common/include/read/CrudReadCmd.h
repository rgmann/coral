#ifndef READ_MSG
#define READ_MSG

#include "GenericPacket.h"

class ReadMsg : public GenericPacket
{
public:

   enum Type
   {
      TypeNotSet
   }

   struct __attribute ((__packed__)) Data
   {
      ui32 type;
      ui32 length;
   };

   ReadMsg();
   
   ReadMsg(ui32 type, ui32 nSizeBytes);

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // READ_MSG
