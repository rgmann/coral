#include <stdio.h>
#include <string.h>
#include "CrudMsg.h"

//------------------------------------------------------------------------------
CrudMsg::CrudMsg()
: GenericPacket()
{
}

//------------------------------------------------------------------------------
CrudMsg::CrudMsg(Type type, ui32 nDataSize)
: CrudMsg(sizeof(CrudMsg::Data) + nDataSize)
{
   if (allocate())
   {
      strncpy(data()->marker, Data::marker, sizeof(Data::marker));
      data()->type   = type;
      data()->length = nDataSize;
   }
}

//------------------------------------------------------------------------------
CrudMsg::CrudMsg(const CreateMsg* pMsg)
{
   from(Create, pMsg);
}

//------------------------------------------------------------------------------
CrudMsg::CrudMsg(const ReadMsg* pMsg)
{
   from(Read, pMsg);
}

//------------------------------------------------------------------------------
CrudMsg::CrudMsg(const CreateMsg* pMsg)
{
   from(Update, pMsg);
}

//------------------------------------------------------------------------------
CrudMsg::CrudMsg(const DeleteMsg* pMsg)
{
   from(Delete, pMsg);
}

//------------------------------------------------------------------------------
CrudMsg::Type CrudMsg::type() const
{
   Type type = TypeNotSet;
   if (isAllocated())
   {
      type = (Type)data()->type;
   }
   
   return type;
}

//------------------------------------------------------------------------------
bool CrudMsg::IsCrudMsg(const GenericPacket* pPkt)
{
   return IsCrudMsg(pPkt->basePtr(), pPkt->allocatedSize());
}

//------------------------------------------------------------------------------
bool CrudMsg::IsCrudMsg(const ui8* pPkt, ui32 nSizeBytes)
{
   if (nSizeBytes < dataSize())
   {
      return false;
   }
   
   Data* lpData = pPkt;
   
   if (strncmp(pPkt->marker, Data::marker, sizeof(Data::marker)) != 0)
   {
      return false;
   }
   
   if (pPkt->type < TypeNotSet)
   {
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool CrudMsg::to(CreateMsg** pMsg) const
{
   if (*pMsg != NULL) return false;
   
   *pMsg = static_cast<CreateMsg*>(create(Create));
   
   return (*pMsg != NULL);
}

//------------------------------------------------------------------------------
bool CrudMsg::to(ReadMsg** pMsg) const
{
   if (*pMsg != NULL) return false;
   
   *pMsg = static_cast<ReadMsg*>(create(Read));
   
   return (*pMsg != NULL);
}

//------------------------------------------------------------------------------
bool CrudMsg::to(UpdateMsg** pMsg) const
{
   if (*pMsg != NULL) return false;
   
   *pMsg = static_cast<UpdateMsg*>(create(Update));
   
   return (*pMsg != NULL);
}

//------------------------------------------------------------------------------
bool CrudMsg::to(DeleteMsg** pMsg) const
{
   if (*pMsg != NULL) return false;
   
   *pMsg = static_cast<DeleteMsg*>(create(Delete));
   
   return (*pMsg != NULL);
}

//------------------------------------------------------------------------------
GenericPacket* CrudMsg::create(Type type) const
{
   GenericPacket* lpPacket = NULL;
   
   if (isAllocated() && ((Type)data()->type == type))
   {
      ui8* lpPayload = reinterpret_cast<ui8*>(basePtr());
      
      lpPayload += dataSize();
      
      switch (type) {
         case Create:
            lpPacket = new CreateMsg();
            break;
            
         case Read:
            lpPacket = new ReadMsg();
            break;
            
         case Update:
            lpPacket = new UpdateMsg();
            break;
            
         case Delete:
            lpPacket = new DeleteMsg();
            break;
            
         default:
            // Bad type. Don't allocate anything.
            break;
      }
      
      if (lpPacket)
      {
         ui32 lnPayloadSize = allocatedSize() - dataSize();
         
         if (!lpPacket->unpack(lpPayload, lnPayloadSize))
         {
            delete lpPacket;
            lpPacket = NULL;
         }
      }
   }
   
   return lpPacket;
}

//------------------------------------------------------------------------------
bool CrudMsg::from(Type type, const GenericPacket* pPacket)
{
   if (!pPacket) return false;
   
   m_nSizeBytes = dataSize() + pPacket->allocatedSize();
   
   if (allocate())
   {
      ui8* l_pPayload = reinterpret_cast<ui8*>(basePtr());
      
      l_pPayload += dataSize();
      
      memcpy(l_pPayload, pPacket->dataPtr(), pPacket->allocatedSize());
      
      data()->type = type;
      data()->length = allocatedSize() - dataSize();
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool CrudMsg::unpack(const void* pPkt, ui32 nSizeBytes)
{
   bool  lbRecognized = false;
   
   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      printf("inherited::unpack: failed\n");
      return false;
   }
   
   // Check that the size is at least large enough for the RsyncPacket's
   // inclusive size.
   if (nSizeBytes < dataSize())
   {
      printf("::unpack: too small\n");
      return false;
   }
   
   // Validate the marker
   if (strncmp(data()->marker, Data::marker, sizeof(Data::marker)) != 0)
   {
      printf("::unpack: bad marker\n");
      return false;
   }
   
   // Validate the packet type field.
   switch (type())
   {
      case Create:
      case Read:
      case Update:
      case Delete:
         lbRecognized = true;
         break;
         
      default:
         lbRecognized = false;
         break;
   }
   
   if (!lbRecognized)
   {
      printf("::unpack: ungrecognized packet ID\n");
      return false;
   }
   
   // Validate the size of the packet against the indicated payload size.
   if ((nSizeBytes - dataSize()) != data()->length)
   {
      printf("::unpack: size mismatch - size = %u, exp = %u\n",
             (nSizeBytes - allocatedSize()), data()->length);
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
CrudMsg::Data* const CrudMsg::data()
{
   if (!isAllocated()) return NULL;
   
   return reinterpret_cast<Data*> (dataPtr());
}

//------------------------------------------------------------------------------
ui32 CrudMsg::dataSize() const
{
   return sizeof(Data);
}
