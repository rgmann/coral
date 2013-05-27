#include <stdio.h>
#include "RsyncSegmentReportPacket.h"

//------------------------------------------------------------------------------
RsyncSegmentReportPacket
::RsyncSegmentReportPacket()
: GenericPacket(sizeof(Data))
{
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket
::RsyncSegmentReportPacket(Type type, ui32 nDataSize)
: GenericPacket(sizeof(Data) + nDataSize)
{
   if (isAllocated())
   {
      data()->type = type;
      data()->length = nDataSize;
   }
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::
RsyncSegmentReportPacket(const RsyncSegmentReportHdr* pPacket)
{
   from(HeaderType, pPacket);
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::
RsyncSegmentReportPacket(const RsyncSegmentPacket* pPacket)
{
   from(SegmentType, pPacket);
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::Type RsyncSegmentReportPacket::type() const
{
   Type type = TypeNotSet;

   if (isAllocated())
   {
      type = (Type)data()->type;
   }
   
   return type;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportPacket::from(Type type, const GenericPacket* pPacket)
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
ui32 RsyncSegmentReportPacket::dataSize() const
{
   return sizeof(Data);
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportPacket::to(RsyncSegmentReportHdr** pPacket)
{
   if (*pPacket != NULL) return false;
   
   *pPacket = static_cast<RsyncSegmentReportHdr*>(create(HeaderType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportPacket::to(RsyncSegmentPacket** pPacket)
{
   if (*pPacket != NULL) return false;
   
   *pPacket = static_cast<RsyncSegmentPacket*>(create(SegmentType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
GenericPacket* RsyncSegmentReportPacket::create(Type type) const
{
   bool l_bSuccess = false;
   GenericPacket* l_pPacket = NULL;
   
   if (isAllocated() && (this->type() == type))
   {
      ui8* l_pPayload = reinterpret_cast<ui8*>(basePtr());
      
      l_pPayload += dataSize();
      
      switch (type) {
         case HeaderType:
            l_pPacket = new RsyncSegmentReportHdr();
            break;
            
         case SegmentType:
            l_pPacket = new RsyncSegmentPacket();
            break;
            
         default:
            // Bad type. Don't allocate anything.
            break;
      }
      
      if (l_pPacket)
      {
         l_bSuccess = l_pPacket->unpack(l_pPayload,
                                        allocatedSize() - dataSize());
         
         if (!l_bSuccess)
         {
            delete l_pPacket;
            l_pPacket = NULL;
         }
      }
   }
   
   return l_pPacket;
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::Data* RsyncSegmentReportPacket::data()
{
   return reinterpret_cast<Data*>(dataPtr());
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::Data* const RsyncSegmentReportPacket::data() const
{
   return reinterpret_cast<Data* const>(dataPtr());
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportPacket::unpack(const void* pPkt, ui32 nSizeBytes)
{
   bool  l_bRecognized = false;
   
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
   
   // Validate the packet type field.
   switch (type())
   {
      case HeaderType:
      case SegmentType:
         l_bRecognized = true;
         break;
         
      default:
         l_bRecognized = true;
         break;
   }
   
   if (!l_bRecognized)
   {
      printf("::unpack: ungrecognized packet ID\n");
      return false;
   }
   
   // Validate the size of the packet against the indicated payload size.
   if ((nSizeBytes - dataSize()) != data()->length)
   {
      printf("::unpack: size mismatch - size = %u, exp = %u\n",
             (nSizeBytes - dataSize()), data()->length);
      return false;
   }
   
   return true;
}
