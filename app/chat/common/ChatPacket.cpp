#include <string.h>
#include <stdio.h>
#include "ChatPacket.h"

//------------------------------------------------------------------------------
ChatPacket::ChatPacket()
{
   m_nHdrSizeBytes = sizeof(ChatPacketHdr);   
}

//------------------------------------------------------------------------------
ChatPacket::ChatPacket(ChatPacketType type, ui32 nDataSize)
: GenericPacket()
{
   m_nHdrSizeBytes = sizeof(ChatPacketHdr);
   
   m_nSizeBytes = m_nHdrSizeBytes + nDataSize;
   
   if (allocate())
   {
      ((ChatPacketHdr*)m_pPkt)->type = type;
      ((ChatPacketHdr*)m_pPkt)->length = m_nSizeBytes - m_nHdrSizeBytes;
   }
}

//------------------------------------------------------------------------------
ChatPacket::ChatPacketType ChatPacket::type() const
{
   ChatPacketType l_type = TypeNotSet;
   
   if (m_pPkt)
   {
      l_type = (ChatPacketType)((ChatPacketHdr*)m_pPkt)->type;
   }
   
   return l_type;
}

//------------------------------------------------------------------------------
bool ChatPacket::unpack(const void* pPkt, unsigned int nSizeBytes)
{
   bool  l_bRecognized = false;
   const ChatPacketHdr* l_pHeader = reinterpret_cast<const ChatPacketHdr*>(pPkt);
   
   if (nSizeBytes < sizeof(ChatPacketHdr))
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
   if (l_pHeader->marker != ChatPacketHdr::marker)
   {
      printf("::unpack: bad marker\n");
      return false;
   }
   
   switch (l_pHeader->type) {
      case ChatMsgType:
      case UpdateRequestType:
      case UpdateResponseType:
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
   if ((nSizeBytes - sizeof(ChatPacketHdr)) != l_pHeader->length)
   {
      printf("::unpack: size mismatch - size = %lu, exp = %u\n",
             (nSizeBytes - sizeof(ChatPacketHdr)),
             l_pHeader->length);
      return false;
   }
   
   // Allocate the packet.
   GenericPacket::unpack(pPkt, nSizeBytes);
   m_nHdrSizeBytes = sizeof(ChatPacketHdr);
   
   // Copy the packet
   memcpy(m_pPkt, pPkt, nSizeBytes);
   
   return true;
}
