#ifndef GENERIC_PACKET_H
#define GENERIC_PACKET_H

#include "BaseTypes.h"

class GenericPacket
{
public:
   
   GenericPacket();
   
   GenericPacket(const GenericPacket &other);
   
   GenericPacket(unsigned int nSizeBytes);
   
   GenericPacket(unsigned int nHdrSizeBytes, unsigned int nDataSizeBytes);
   
   virtual ~GenericPacket();
   
   bool  allocate();
   
   void  deallocate();
   
   bool  isAllocated() const;
   
   void  destroy();
   
   unsigned int size() const;
   
   unsigned int hdrSize() const;
      
   unsigned int dataSize() const;
   
   void* const hdrPtr() const;
   
   void* const dataPtr() const;
   
   virtual bool  pack(void** pData, unsigned int &nSizeBytes) const;
   
   virtual bool  unpack(const void* pData, unsigned int nSizeBytes);
   
   GenericPacket& operator= (const GenericPacket& other);
   
protected:
   
   void* dataPtr();
   
protected:
   
   unsigned char* m_pPkt;
   
   unsigned int   m_nHdrSizeBytes;
   
   unsigned int   m_nSizeBytes;
};

#endif // GENERIC_PACKET_H