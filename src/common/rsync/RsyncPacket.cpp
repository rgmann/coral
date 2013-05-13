#include <string.h>
#include <stdio.h>
#include "RsyncPacket.h"

//------------------------------------------------------------------------------
RsyncPacket::RsyncPacket()
: GenericPacket(sizeof(RsyncPacket::Data), 0)
{
   //m_nHdrSizeBytes = sizeof(Data);
}

//------------------------------------------------------------------------------
RsyncPacket::RsyncPacket(Type type, ui32 nDataSize)
: GenericPacket(sizeof(RsyncPacket::Data), nDataSize)
{
//   m_nHdrSizeBytes = sizeof(Data);
//   
//   m_nSizeBytes = m_nHdrSizeBytes + nDataSize;
   
   if (allocate())
   {
//      ((Data*)m_pPkt)->type = type;
//      ((Data*)m_pPkt)->length = m_nSizeBytes - m_nHdrSizeBytes;
      Data* const l_pHeader = reinterpret_cast<Data* const>(header());
      
      l_pHeader->type = type;
      l_pHeader->length = nDataSize;
   }
}

//------------------------------------------------------------------------------
RsyncPacket::Type RsyncPacket::type() const
{
//   RsyncPacketType l_type = TypeNotSet;
   Type l_type = TypeNotSet;
   
   //if (m_pPkt)
   if (header())
   {
//      l_type = (Type)((Data*)m_pPkt)->type;
      Data* const l_pHeader = reinterpret_cast<Data* const>(header());
      
      l_type = (Type)l_pHeader->type;
   }
   
   return l_type;
}

//------------------------------------------------------------------------------
bool RsyncPacket::unpack(const void* pPkt, unsigned int nSizeBytes)
{
   bool  l_bRecognized = false;
   const Data* l_pHeader = reinterpret_cast<const Data*>(pPkt);
   
   // The packet must at least be as large as the RsyncPacket header.
//   if (nSizeBytes < sizeof(Data))
//   {
//      printf("RsyncPacket::unpack: too small\n");
//      return false;
//   }
   
   if (pPkt == NULL)
   {
      printf("::unpack: NULL\n");
      return false;
   }
   
   if (!inherited::unpack(pPkt, nSizeBytes))
   {
      printf("inherited::unpack: failed\n");
      return false;
   }
   
   if (nSizeBytes < (headerSize() + dataSize()))
   {
      printf("::unpack: too small\n");
      return false;
   }
   
   // Validate the marker
//   if (l_pHeader->Marker != Data::Marker)
//   {
//      printf("::unpack: bad marker\n");
//      return false;
//   }
   
//   switch (l_pHeader->type) {
   switch (type())
   {
      case SegmentReportType:
      case AssemblyInstType:
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
   if ((nSizeBytes - sizeof(Data)) != l_pHeader->length)
   {
      printf("::unpack: size mismatch - size = %lu, exp = %u\n",
             (nSizeBytes - sizeof(Data)), l_pHeader->length);
      return false;
   }
   
   // Allocate/reallocate the packet.
   GenericPacket::unpack(pPkt, nSizeBytes);
   m_nHdrSizeBytes = sizeof(Data);
   
   // Copy the packet
   memcpy(m_pPkt, pPkt, nSizeBytes);
   
   return true;
}

//------------------------------------------------------------------------------
ui32 RsyncPacket::headerSize() const
{
   return inherited::headerSize() + dataSize();
}
