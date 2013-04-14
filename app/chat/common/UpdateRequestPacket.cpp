#include <string.h>
#include "UpdateRequestPacket.h"

//------------------------------------------------------------------------------
UpdateRequestPacket::UpdateRequestPacket()
: ChatPacket(UpdateRequestType, sizeof(UpdateRequestData))
{
   if (dataPtr())
   {
      memset(dataPtr(), 0, sizeof(UpdateRequestData));
   }
}

//------------------------------------------------------------------------------
UpdateRequestPacket::UpdateRequestPacket(ui32 userId, ui32 lastTs)
: ChatPacket(UpdateRequestType, sizeof(UpdateRequestData))
{
   UpdateRequestData* l_pData = NULL;
   
   if (dataPtr())
   {
      l_pData = reinterpret_cast<UpdateRequestData*>(dataPtr());
      l_pData->user_id = userId;
      l_pData->timestamp = lastTs;
   }
}

//------------------------------------------------------------------------------
bool UpdateRequestPacket::setId(ui32 userId)
{
   UpdateRequestData* l_pData = NULL;
   
   l_pData = reinterpret_cast<UpdateRequestData*>(dataPtr());
   
   if (l_pData == NULL)
   {
      return false;
   }
   
   l_pData->user_id = userId;
   
   return true;
}

//------------------------------------------------------------------------------
bool UpdateRequestPacket::getId(ui32 &userId) const
{
   UpdateRequestData* l_pData = reinterpret_cast<UpdateRequestData*>(dataPtr());
   
   if (l_pData == NULL)
   {
      return false;
   }
   
   userId = l_pData->user_id;
   
   return true;
}

//------------------------------------------------------------------------------
bool UpdateRequestPacket::setTs(ui32 lastTs)
{
   UpdateRequestData* l_pData = reinterpret_cast<UpdateRequestData*>(dataPtr());
   
   if (l_pData == NULL)
   {
      return false;
   }
   
   l_pData->timestamp = lastTs;
   
   return true;
}

//------------------------------------------------------------------------------
bool UpdateRequestPacket::getTs(ui32 &lastTs) const
{
   UpdateRequestData* l_pData = reinterpret_cast<UpdateRequestData*>(dataPtr());
   
   if (l_pData == NULL)
   {
      return false;
   }
   
   lastTs = l_pData->timestamp;
   
   return true;
}
