#ifndef DELETE_MSG
#define DELETE_MSG

#include "GenericPacket.h"

class DeleteMsg : public GenericPacket
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

   DeleteMsg();
   
   DeleteMsg(ui32 type, ui32 nSizeBytes);
   
   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // DELETE_MSG
