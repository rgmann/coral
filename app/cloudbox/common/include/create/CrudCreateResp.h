#ifndef CB_CREATE_MSG_H
#define CB_CREATE_MSG_H

#include "CreateMsg.h"

class CbCreateMsg : public CreateMsg
{
public:

   enum Type
   {
      CreateFile,
      CreateGroup,
      CreateUser,
      TypeNotSet
   }

   CbCreateMsg();
   CbCreateMsg(Type type, ui32 nSizeBytes);

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef CreateMsg inherited;

};

#endif // CB_CREATE_MSG_H
