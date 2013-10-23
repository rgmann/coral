#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GenericPacket.h"

//------------------------------------------------------------------------------
GenericPacket::GenericPacket()
   : m_pPkt(NULL)
   , m_nSizeBytes(0)
{
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(const GenericPacket &other)
: m_pPkt(NULL)
, m_nSizeBytes(0)
{
   *this = other;
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(ui32 nSizeBytes)
   : m_pPkt(NULL)
   , m_nSizeBytes(nSizeBytes)
{
   allocate();
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
   
   m_pPkt = new unsigned char[m_nSizeBytes];
   
   return (m_pPkt != NULL);
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate(ui32 sizeBytes)
{
   m_nSizeBytes = sizeBytes;
   return allocate();
}

//------------------------------------------------------------------------------
bool GenericPacket::isAllocated() const
{
   return (basePtr() != NULL);
}

//------------------------------------------------------------------------------
void GenericPacket::deallocate()
{
   if (m_pPkt)
   {
      delete[] m_pPkt;
      m_pPkt = NULL;
   }
   
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
ui32 GenericPacket::allocatedSize() const
{
   return m_nSizeBytes;
}

//------------------------------------------------------------------------------
//ui32 GenericPacket::dataOffset() const
//{
//   return 0;
//}

//------------------------------------------------------------------------------
ui32 GenericPacket::dataSize() const
{
   return sizeof(Data);
}

//------------------------------------------------------------------------------
//ui32 GenericPacket::inclusiveSize() const
//{
//   return 0;
//}

//------------------------------------------------------------------------------
void* GenericPacket::basePtr()
{
   return reinterpret_cast<void*>(m_pPkt);
}

//------------------------------------------------------------------------------
void* const GenericPacket::basePtr() const
{
   return reinterpret_cast<void* const>(m_pPkt);
}

//------------------------------------------------------------------------------
void* GenericPacket::dataPtr()
{
//   printf("GenericPacket::dataPtr: offset = %u\n", dataOffset());
   if (m_pPkt)
   {
//      return reinterpret_cast<void*>(m_pPkt + dataOffset());
      return basePtr();
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
void* const GenericPacket::dataPtr() const
{
   if (m_pPkt)
   {
//      printf("const GenericPacket::dataPtr: offset = %u\n", dataOffset());
//      return reinterpret_cast<void* const>(m_pPkt + dataOffset());
      return basePtr();
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
//void* GenericPacket::dataEndPtr()
//{
//   printf("GenericPacket::dataEndPtr\n");
//   if (isAllocated())
//   {
//      return reinterpret_cast<void*>(m_pPkt + dataOffset() + dataSize());
//   }
//   
//   return NULL;
//}

//------------------------------------------------------------------------------
//void* const GenericPacket::dataEndPtr() const
//{
//   printf("GenericPacket::dataEndPtr\n");
//   if (isAllocated())
//   {
//      return reinterpret_cast<void* const>(m_pPkt + dataOffset() + dataSize());
//   }
//   
//   return NULL;
//}

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
   *pPkt = new unsigned char[allocatedSize()];
   if (*pPkt == NULL)
   {
      printf("GenericPacket::pack: Failed to allocate pointer\n");
      nSizeBytes = 0;
      return false;
   }
   
//   memcpy(l_pPkt, m_pPkt, size());
   memcpy(*pPkt, m_pPkt, allocatedSize());
   nSizeBytes = allocatedSize();
      
   return true;
}

//------------------------------------------------------------------------------
bool GenericPacket::unpack(const void* pPkt, ui32 nSizeBytes)
{
   if (pPkt == NULL)
   {
      printf("::unpack: NULL\n");
      return false;
   }
   
   // Begin by destroying the packet if it is already allocated.
   destroy();
   
   // The header size is known since it is supplied...
   m_nSizeBytes = nSizeBytes;
   
   if (!allocate())
   {
      printf("GenericPacket::unpack: Allocation failed\n");
      return false;
   }
   
   // Copy the packet.
   memcpy(m_pPkt, pPkt, allocatedSize());
   
   return true;
}

//------------------------------------------------------------------------------
GenericPacket& GenericPacket::operator= (const GenericPacket &other)
{
   // Check for self-assignment!
   if (this == &other)
      return *this;
   
   deallocate();
   
   m_nSizeBytes = other.m_nSizeBytes;
   
   if (allocate())
   {
      memcpy(m_pPkt, other.m_pPkt, allocatedSize());
   }
   
   return *this;
}
