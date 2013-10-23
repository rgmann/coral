#include <string.h>
#include <stdio.h>
#include "RsyncPacket.h"

//------------------------------------------------------------------------------
RsyncPacket::RsyncPacket()
: GenericPacket(sizeof(RsyncPacket::Data))
{
}

//------------------------------------------------------------------------------
RsyncPacket::RsyncPacket(Type type, ui32 nDataSize)
: GenericPacket(sizeof(RsyncPacket::Data) + nDataSize)
{
   if (allocate())
   {
      data()->marker = RsyncPacketMarker;
      data()->type   = type;
      data()->length = nDataSize;
   }
}

//------------------------------------------------------------------------------
RsyncPacket::
RsyncPacket(const RsyncAssemblyInstr* pPacket)
{
   from(AssemblyInstType, pPacket);
}

//------------------------------------------------------------------------------
RsyncPacket::
RsyncPacket(const RsyncSegmentReportPacket* pPacket)
{
   from(SegmentReportType, pPacket);
}

//------------------------------------------------------------------------------
RsyncPacket::Type RsyncPacket::type() const
{
   Type type = TypeNotSet;
   printf("RsyncPacket::type\n");
   if (isAllocated())
   {
      type = (Type)data()->type;
   }
   
   return type;
}

//------------------------------------------------------------------------------
bool RsyncPacket::to(RsyncAssemblyInstr** pPacket) const
{
   if (*pPacket != NULL) return false;
   
   *pPacket = static_cast<RsyncAssemblyInstr*>(create(AssemblyInstType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
bool RsyncPacket::to(RsyncSegmentReportPacket** pPacket) const
{
   if (*pPacket != NULL) return false;
   
   *pPacket = static_cast<RsyncSegmentReportPacket*>(create(SegmentReportType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
GenericPacket* RsyncPacket::create(Type type) const
{
   bool l_bSuccess = false;
   GenericPacket* l_pPacket = NULL;
   
   if (isAllocated() && ((Type)data()->type == type))
   {
      ui8* l_pPayload = reinterpret_cast<ui8*>(basePtr());
      
      l_pPayload += dataSize();
      
      switch (type) {
         case SegmentReportType:
            l_pPacket = new RsyncAssemblyInstr();
            break;
            
         case AssemblyInstType:
            l_pPacket = new RsyncSegmentReportPacket();
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
            printf("RsyncAssemblyInstr::create: failed to allocate packet\n");
            delete l_pPacket;
            l_pPacket = NULL;
         }
      }
   }
   
   return l_pPacket;
}

//------------------------------------------------------------------------------
bool RsyncPacket::from(Type type, const GenericPacket* pPacket)
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
RsyncPacket::Data* const RsyncPacket::data() const
{
   printf("RsyncPacket::data\n");
   return reinterpret_cast<Data* const>(dataPtr());
}

//------------------------------------------------------------------------------
ui32 RsyncPacket::dataSize() const
{
   return sizeof(Data);
}

//------------------------------------------------------------------------------
bool RsyncPacket::unpack(const void* pPkt, ui32 nSizeBytes)
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
   
   // Validate the marker
   if (data()->marker != RsyncPacketMarker)
   {
      printf("::unpack: bad marker\n");
      return false;
   }
   
   // Validate the packet type field.
   switch (type())
   {
      case SegmentReportType:
      case AssemblyInstType:
         l_bRecognized = true;
         break;
         
      default:
         l_bRecognized = false;
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
             (nSizeBytes - allocatedSize()), data()->length);
      return false;
   }
   
   return true;
}

