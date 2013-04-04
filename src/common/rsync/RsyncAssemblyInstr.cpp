#include "RsyncAssemblyInstr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
RsyncAssemblyInstr::RsyncAssemblyInstr() : GenericPacket()
{
   // If the default contructor is called, the packet is allocated to the size
   // of the header.  The packet can be resized later with a call to setType().
   m_nHdrSizeBytes = m_nSizeBytes = sizeof(RsyncAssemblyInstrHdr);
   
   if (allocate())
   {
      ((RsyncAssemblyInstrHdr*)m_pPkt)->type = RsyncTypeNotSet;
   }
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::RsyncAssemblyInstr(RsyncAssmbInstrType type,
                                       unsigned int nParam)
{
   m_nHdrSizeBytes = sizeof(RsyncAssemblyInstrHdr);
   
   if (type == RsyncChunkType)
   {
      m_nSizeBytes = m_nHdrSizeBytes + nParam;
   }
   else if (type == RsyncBeginMarker)
   {
      m_nSizeBytes = m_nHdrSizeBytes + nParam;
   }
   else
   {
      m_nSizeBytes = m_nHdrSizeBytes;
   }
   
   if (allocate())
   {
      ((RsyncAssemblyInstrHdr*)m_pPkt)->type = type;
      ((RsyncAssemblyInstrHdr*)m_pPkt)->info.generic = nParam;
      printf("RsyncAssemblyInstr: ttl=%u, parm=%u\n",
             m_nSizeBytes, nParam);
   }
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::~RsyncAssemblyInstr()
{
   destroy();
}

//------------------------------------------------------------------------------
RsyncAssemblyInstr::RsyncAssmbInstrType RsyncAssemblyInstr::type() const
{
   RsyncAssmbInstrType l_type = RsyncTypeNotSet;
   
   if (m_pPkt)
   {
      l_type = (RsyncAssmbInstrType)((RsyncAssemblyInstrHdr*)m_pPkt)->type;
   }
   
   return l_type;
}

//------------------------------------------------------------------------------
unsigned int RsyncAssemblyInstr::info() const
{
   unsigned int l_nGeneric = 0;
   
   if (m_pPkt)
   {
      l_nGeneric = ((RsyncAssemblyInstrHdr*)m_pPkt)->info.generic;
   }
   
   return l_nGeneric;
}

//------------------------------------------------------------------------------
bool RsyncAssemblyInstr::unpack(void* pPkt, unsigned int nSizeBytes)
{
   RsyncAssemblyInstrHdr* l_pHeader = (RsyncAssemblyInstrHdr*)pPkt;
   
   if (nSizeBytes < sizeof(RsyncAssemblyInstrHdr))
   {
      printf("::unpack: too small\n");
      return false;
   }
   
   if (pPkt == NULL)
   {
      printf("::unpack: NULL\n");
      return false;
   }
   
   // Validate the marker
   if (l_pHeader->marker != RsyncAssemblyInstrHdr::marker)
   {
      printf("::unpack: bad marker\n");
      return false;
   }
   
   // If this is a chunk instruction, allocate the data section.
   if (l_pHeader->type == RsyncChunkType)
   {
      // Validate the size of the packet against the indicated payload size.
      if ((nSizeBytes - sizeof(RsyncAssemblyInstrHdr)) !=
          l_pHeader->info.chunkSizeBytes)
      {
         printf("::unpack: size mismatch - size = %d, exp = %d\n",
                (nSizeBytes - sizeof(RsyncAssemblyInstrHdr)),
                l_pHeader->info.chunkSizeBytes);
         return false;
      }
   }
   else if (l_pHeader->type == RsyncBeginMarker)
   {
      // Validate the size of the packet against the indicated payload size.
      if ((nSizeBytes - sizeof(RsyncAssemblyInstrHdr)) !=
          l_pHeader->info.relPathLen)
      {
         printf("::unpack: size mismatch - size = %d, expected = %d\n",
                (nSizeBytes - sizeof(RsyncAssemblyInstrHdr)),
                l_pHeader->info.relPathLen);
         return false;
      }
   }
   
   // Allocate the packet.
   GenericPacket::unpack(pPkt, nSizeBytes);
   m_nHdrSizeBytes = sizeof(RsyncAssemblyInstrHdr);
   
   // Copy the packet
   memcpy(m_pPkt, pPkt, nSizeBytes);
   
   return true;
}
