#include "GenericPacket.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
GenericPacket::GenericPacket()
   : m_pPkt(NULL)
   //, m_nHdrSizeBytes(0)
   , m_nSizeBytes(0)
{
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(const GenericPacket &other)
{
   *this = other;
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(ui32 nSizeBytes)
   : m_pPkt(NULL)
   //, m_nHdrSizeBytes(0)
   , m_nSizeBytes(nSizeBytes)
{
}

//------------------------------------------------------------------------------
//GenericPacket::GenericPacket(ui32 nHdrSizeBytes, ui32 nDataSizeBytes)
//   : m_pPkt(NULL)
//   , m_nHdrSizeBytes(nHdrSizeBytes)
//   , m_nSizeBytes(nHdrSizeBytes + nDataSizeBytes)
//{
//}

//------------------------------------------------------------------------------
GenericPacket::~GenericPacket()
{
   deallocate();
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate()
{
   if (m_pPkt != NULL || m_nSizeBytes == 0)
   {
      return false;
   }
   
//   if (m_nHdrSizeBytes > m_nSizeBytes)
//   {
//      return false;
//   }
   
   m_pPkt = new unsigned char[m_nSizeBytes];
   
   return (m_pPkt != NULL);
}

//------------------------------------------------------------------------------
void GenericPacket::deallocate()
{
   if (m_pPkt)
   {
      delete[] m_pPkt;
      m_pPkt = NULL;
   }
   
   //m_nHdrSizeBytes = 0;
   m_nSizeBytes = 0;
}

//------------------------------------------------------------------------------
void GenericPacket::destroy()
{
   if (m_pPkt)
   {
      delete[] m_pPkt;
      m_pPkt = NULL;
   }
}

//------------------------------------------------------------------------------
ui32 GenericPacket::size() const
{
   return m_nSizeBytes;
}

//------------------------------------------------------------------------------
ui32 GenericPacket::headerSize() const
{
   return 0;
}

//------------------------------------------------------------------------------
ui32 GenericPacket::dataSize() const
{
//   return size() - hdrSize();
   return size();
}

//------------------------------------------------------------------------------
//void* GenericPacket::hdrPtr()
//{
//   return (void*)m_pPkt;
//}

//------------------------------------------------------------------------------
//void* const GenericPacket::header() const
//{
//   return reinterpret_cast<void* const>(m_pPkt);
//}

//------------------------------------------------------------------------------
void* GenericPacket::dataPtr()
{
//   if (m_nHdrSizeBytes < m_nSizeBytes)
//   {
//      return (void*)(m_pPkt + m_nHdrSizeBytes);
//   }
   if (m_pPkt)
   {
      return reinterpret_cast<void*>(m_pPkt);
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
void* const GenericPacket::dataPtr() const
{
   //   if (m_nHdrSizeBytes < m_nSizeBytes)
   //   {
   //      return (void*)(m_pPkt + m_nHdrSizeBytes);
   //   }
   if (m_pPkt)
   {
      return reinterpret_cast<void* const>(m_pPkt);
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
//void* GenericPacket::dataEndPtr()
//{
//   if (m_pPkt)
//   {
//      return reinterpret_cast<void*>(m_pPkt + size());
//   }
//   
//   return NULL;
//}

//------------------------------------------------------------------------------
//void* const GenericPacket::dataEndPtr() const
//{
//   if (m_pPkt)
//   {
//      return reinterpret_cast<void* const>(m_pPkt + size());
//   }
//   
//   return NULL;
//}

//------------------------------------------------------------------------------
void* const GenericPacket::data() const
{
//   if (m_nHdrSizeBytes < m_nSizeBytes)
//   {
//      return reinterpret_cast<void* const>(m_pPkt + m_nHdrSizeBytes);
//   }
   
//   if (m_pPkt)
//   {
//      return reinterpret_cast<void* const>(m_pPkt);
//   }
//   
//   return NULL;
   return dataPtr();
}

//------------------------------------------------------------------------------
bool GenericPacket::pack(void** pPkt, ui32 &nSizeBytes) const
{
   //unsigned char* l_pPkt = (unsigned char*)(*pPkt);
   
   //if (l_pPkt != NULL)
   if (*pPkt != NULL)
   {
      printf("GenericPacket::pack: Pointer already allocated\n");
      return false;
   }
   
//   l_pPkt = new unsigned char[size()];
   *pPkt = new unsigned char[size()];
//   if (l_pPkt == NULL)
   if (*pPkt == NULL)
   {
      printf("GenericPacket::pack: Failed to allocate pointer\n");
      nSizeBytes = 0;
      return false;
   }
   
//   memcpy(l_pPkt, m_pPkt, size());
   memcpy(*pPkt, m_pPkt, size());
   nSizeBytes = size();
   
   //printf("GenericPacket::pack: Allocated and copied %d\n", size());
   
   return true;
}

//------------------------------------------------------------------------------
bool GenericPacket::unpack(const void* pPkt, ui32 nSizeBytes)
{
   // Begin by destroying the packet if it is already allocated.
   destroy();
   
   // The header size is known since it is supplied...
   m_nSizeBytes = nSizeBytes;
   
   // ...however we have no idea what the header or data sizes are for
   // a generic packet.  Therefore, set to zero.
   //m_nHdrSizeBytes = 0;
   
   if (!allocate())
   {
      return false;
   }
   
   // NOTE: This function only allocates to the correct size.  It does not
   // copy the packet since derived classes may want to perform more validation.
   
   return true;
}

//------------------------------------------------------------------------------
GenericPacket& GenericPacket::operator= (const GenericPacket &other)
{
   // Check for self-assignment!
   if (this == &other)
      return *this;
   
   deallocate();
   
   //m_nHdrSizeBytes   = other.m_nHdrSizeBytes;
   m_nSizeBytes      = other.m_nSizeBytes;
   
   if (allocate())
   {
//      memcpy(m_pPkt, other.m_pPkt, m_nHdrSizeBytes);
      memcpy(m_pPkt, other.m_pPkt, size());
   }
   
   return *this;
}
