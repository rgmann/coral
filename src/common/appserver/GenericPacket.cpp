#include "GenericPacket.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
GenericPacket::GenericPacket()
   : m_pPkt(NULL)
   , m_nHdrSizeBytes(0)
   , m_nSizeBytes(0)
{
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(const GenericPacket &other)
{
   *this = other;
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(unsigned int nSizeBytes)
   : m_pPkt(NULL)
   , m_nHdrSizeBytes(0)
   , m_nSizeBytes(nSizeBytes)
{
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(unsigned int nHdrSizeBytes,
                             unsigned int nDataSizeBytes)
   : m_pPkt(NULL)
   , m_nHdrSizeBytes(nHdrSizeBytes)
   , m_nSizeBytes(nHdrSizeBytes + nDataSizeBytes)
{
}

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
   
   if (m_nHdrSizeBytes > m_nSizeBytes)
   {
      return false;
   }
   
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
   
   m_nHdrSizeBytes = 0;
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
unsigned int GenericPacket::size() const
{
   return m_nSizeBytes;
}

//------------------------------------------------------------------------------
unsigned int GenericPacket::hdrSize() const
{
   return m_nHdrSizeBytes;
}

//------------------------------------------------------------------------------
unsigned int GenericPacket::dataSize() const
{
   return size() - hdrSize();
}

//------------------------------------------------------------------------------
void* const GenericPacket::hdrPtr() const
{
   return (void*)m_pPkt;
}

//------------------------------------------------------------------------------
void* GenericPacket::dataPtr()
{
   if (m_nHdrSizeBytes < m_nSizeBytes)
   {
      return (void*)(m_pPkt + m_nHdrSizeBytes);
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
void* const GenericPacket::dataPtr() const
{
   if (m_nHdrSizeBytes < m_nSizeBytes)
   {
      return reinterpret_cast<void* const>(m_pPkt + m_nHdrSizeBytes);
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
bool GenericPacket::pack(void** pPkt, unsigned int &nSizeBytes) const
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
   
   printf("GenericPacket::pack: Allocated and copied %d\n", size());
   
   return true;
}

//------------------------------------------------------------------------------
bool GenericPacket::unpack(const void* pPkt, unsigned int nSizeBytes)
{
   // Begin by destroying the packet if it is already allocated.
   destroy();
   
   // The header size is known since it is supplied...
   m_nSizeBytes = nSizeBytes;
   
   // ...however we have no idea what the header or data sizes are for
   // a generic packet.  Therefore, set to zero.
   m_nHdrSizeBytes = 0;
   
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
   
   m_nHdrSizeBytes   = other.m_nHdrSizeBytes;
   m_nSizeBytes      = other.m_nSizeBytes;
   
   if (allocate())
   {
      memcpy(m_pPkt, other.m_pPkt, m_nHdrSizeBytes);
   }
   
   return *this;
}
