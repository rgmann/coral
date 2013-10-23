#ifndef UPDATE_MSG
#define UPDATE_MSG

#include "GenericPacket.h"

class UpdateMsg : public GenericPacket
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

   UpdateMsg();
   
   UpdateMsg(ui32 type, ui32 nSizeBytes);

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // UPDATE_MSG
