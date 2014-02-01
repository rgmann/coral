#include "RsyncAssemblyInstr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
RsyncAssemblyInstr::RsyncAssemblyInstr()
: GenericPacket(sizeof(RsyncAssemblyInstr::Data))
{
   if (isAllocated())
   {
      data()->type = TypeNotSet;
   }
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::
RsyncAssemblyInstr(const AssemblyBeginMarker* pPacket)
{
   from(BeginMarkerType, pPacket);
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::
RsyncAssemblyInstr(const AssemblyEndMarker* pPacket)
{
   from(EndMarkerType, pPacket);
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::
RsyncAssemblyInstr(const AssemblyChunkPacket* pPacket)
{
   from(ChunkType, pPacket);
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::
RsyncAssemblyInstr(const AssemblySegmentPacket*  pPacket)
{
   from(SegmentType, pPacket);
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::~RsyncAssemblyInstr()
{
   destroy();
}

//------------------------------------------------------------------------------
bool RsyncAssemblyInstr::from(Type type, const GenericPacket* pPacket)
{
   if (!pPacket) return false;
   
   m_nSizeBytes = dataSize() + pPacket->allocatedSize();
      
   if (allocate())
   {
      ui8* l_pPayload = reinterpret_cast<ui8*>(basePtr());
      
      l_pPayload +=  dataSize();
      
      memcpy(l_pPayload, pPacket->dataPtr(), pPacket->allocatedSize());
         
      data()->type = type;
      data()->length = allocatedSize() - dataSize();
   }
   
   return true;
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::Type RsyncAssemblyInstr::type() const
{
   Type type = TypeNotSet;
   
   if (isAllocated())
   {
      type = (Type)data()->type;
   }
   
   return type;
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::Data* const RsyncAssemblyInstr::data() const
{
   if (isAllocated())
   {
      Data* const l_pData = reinterpret_cast<Data* const>(dataPtr());
      return l_pData;
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
ui32 RsyncAssemblyInstr::dataSize() const
{
   return sizeof(Data);
}

//------------------------------------------------------------------------------
bool RsyncAssemblyInstr::to(AssemblyBeginMarker** pPacket) const
{
   if (*pPacket != NULL) return false;

   *pPacket = static_cast<AssemblyBeginMarker*>(create(BeginMarkerType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
bool RsyncAssemblyInstr::to(AssemblyEndMarker** pPacket) const
{
   if (*pPacket != NULL) return false;

   *pPacket = static_cast<AssemblyEndMarker*>(create(EndMarkerType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
bool RsyncAssemblyInstr::to(AssemblyChunkPacket** pPacket) const
{
   if (*pPacket != NULL) return false;

   *pPacket = static_cast<AssemblyChunkPacket*>(create(ChunkType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
bool RsyncAssemblyInstr::to(AssemblySegmentPacket** pPacket) const
{ 
   if (*pPacket != NULL) return false;

   *pPacket = static_cast<AssemblySegmentPacket*>(create(SegmentType));
   
   return (*pPacket != NULL);
}

//------------------------------------------------------------------------------
GenericPacket* RsyncAssemblyInstr::create(Type type) const
{
   bool l_bSuccess = false;
   GenericPacket* l_pPacket = NULL;
   
   if (isAllocated() && ((Type)data()->type == type))
   {
      ui8* l_pPayload = reinterpret_cast<ui8*>(basePtr());
      
      l_pPayload += dataSize();
      
      switch (type) {
         case BeginMarkerType:
            l_pPacket = new AssemblyBeginMarker();
            printf("RsyncAssemblyInstr::create: Creating BeginMarker\n");
            break;
            
         case EndMarkerType:
            l_pPacket = new AssemblyEndMarker();
            break;
            
         case ChunkType:
            l_pPacket = new AssemblyChunkPacket();
            break;
            
         case SegmentType:
            l_pPacket = new AssemblySegmentPacket();
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
bool RsyncAssemblyInstr::unpack(const void* pPkt, ui32 nSizeBytes)
{
   bool l_bRecognized = false;

   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      printf("RsyncAssemblyInstr::unpack: inherited unpack failed.\n");
      return false;
   }
   
   // Verify that the packet is at least large enough to contain the
   // RsyncAssemblyInstr data segment and all preceding data segments.
   if (nSizeBytes < dataSize())
   {
      printf("::unpack: too small\n");
      return false;
   }
   
   // Validate the packet type.
   switch (type())
   {
      case BeginMarkerType:
      case EndMarkerType:
      case ChunkType:
      case SegmentType:
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
             (nSizeBytes - dataSize()), data()->length);
      return false;
   }
   
   return true;
}
