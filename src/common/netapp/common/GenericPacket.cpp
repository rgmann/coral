#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Log.h"
#include "GenericPacket.h"

using namespace liber::netapp;

//------------------------------------------------------------------------------
GenericPacket::GenericPacket()
: m_pPkt(NULL)
, mnDataSizeBytes(0)
, m_nSizeBytes(0)
{
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(const GenericPacket &other)
: m_pPkt(NULL)
, mnDataSizeBytes(0)
, m_nSizeBytes(0)
{
   *this = other;
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(ui32 nSizeBytes)
: m_pPkt(NULL)
, mnDataSizeBytes(0)
, m_nSizeBytes(nSizeBytes)
{
   allocate();
}

//------------------------------------------------------------------------------
GenericPacket::GenericPacket(ui32 nDataSizeBytes, ui32 nBodySizeBytes)
: m_pPkt(NULL)
, mnDataSizeBytes(nDataSizeBytes)
, m_nSizeBytes(nDataSizeBytes + nBodySizeBytes)
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
   if (m_nSizeBytes == 0)
   {
      log::debug("GenericPacket::allocate: Already allocated!\n");
      return false;
   }

  if (m_pPkt)
  {
    delete[] m_pPkt;
    m_pPkt = NULL;
  }
   
   m_pPkt = new unsigned char[m_nSizeBytes];
   memset(m_pPkt, 0, m_nSizeBytes);
   
   return (m_pPkt != NULL);
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate(ui32 sizeBytes)
{
   mnDataSizeBytes = 0;
   m_nSizeBytes = sizeBytes;
   return allocate();
}

//------------------------------------------------------------------------------
bool GenericPacket::allocate(ui32 nDataSizeBytes, ui32 nBodySizeBytes)
{
  mnDataSizeBytes = nDataSizeBytes;
  m_nSizeBytes = nDataSizeBytes + nBodySizeBytes;
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
ui32 GenericPacket::dataSize() const
{
   return mnDataSizeBytes;
}

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
   char* lpData = NULL;

   if (m_pPkt)
   {
      lpData = reinterpret_cast<char*>(basePtr());
      lpData += mnDataSizeBytes;
   }
   
   return lpData;
}

//------------------------------------------------------------------------------
void* const GenericPacket::dataPtr() const
{
   char* lpData = NULL;
   
   if (m_pPkt)
   {
      lpData = reinterpret_cast<char*>(basePtr());
      lpData += mnDataSizeBytes;
   }

   return (void* const)lpData;
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
  bool lbSuccess = false;

  if (pPkt != NULL)
  {
    ui32 lnDataSizeBytes = mnDataSizeBytes;

    if (nSizeBytes >= mnDataSizeBytes)
    {
      // Begin by destroying the packet if it is already allocated.
      destroy();

      // The header size is known since it is supplied...
      if (allocate(lnDataSizeBytes, nSizeBytes - lnDataSizeBytes))
      {
        // Copy the packet.
        memcpy(m_pPkt, pPkt, allocatedSize());
        lbSuccess = true;
      }
      else
      {
        log::error("GenericPacket::unpack: Allocation failed\n");
      }
    }
    else
    {
      log::error("GenericPacket::unpack - "
                 "Buffer smaller than minimum data size.\n");
    }
  }
  else
  {
    log::error("GenericPacket::unpack: NULL\n");
  }
   
  return lbSuccess;
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

//-----------------------------------------------------------------------------
void GenericPacket::swapByteOrder()
{
  swap(m_pPkt, allocatedSize());
}

