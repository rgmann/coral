#ifndef CRUD_MSG_H
#define CRUD_MSG_H

#include "CreateMsg.h"
#include "ReadMsg.h"
#include "UpdateMsg.h"
#include "DeleteMsg.h"

class CrudMsg : public GenericPacket
{
public:

   enum Type
   {
      Create,
      Read,
      Update,
      Delete,
      TypeNotSet
   }

   struct __attribute ((__packed__)) Data
   {
      static const char marker[] = "crud";
      ui32 type;
      ui32 length;
   };

   CrudMsg();
   CrudMsg(Type type, ui32 nSizeBytes);
   CrudMsg(const CreateMsg* pMsg);
   CrudMsg(const ReadMsg* pMsg);
   CrudMsg(const UpdateMsg* pMsg);
   CrudMsg(const DeleteMsg* pMsg);
   
   bool to(CreateMsg** pMsg);
   bool to(ReadMsg** pMsg);
   bool to(CreateMsg** pMsg);
   bool to(UpdateMsg** pMsg);
   
   Type type() const;
   
   static bool IsCrudMsg(const GenericPacket* pPkt);
   static bool IsCrudMsg(const ui8* pPkt, ui32 nSizeBytes);

   bool  unpack(void* pPkt, ui32 nSizeBytes);
   
   Data* const data();
   
private:
   
   virtual ui32  dataSize() const;
   
   GenericPacket* create(Type type) const;
   
   bool from(Type type, const GenericPacket* pPacket);

private:

   typedef GenericPacket inherited;

};

#endif // CRUD_MSG_H
