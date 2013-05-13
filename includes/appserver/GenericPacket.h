#ifndef GENERIC_PACKET_H
#define GENERIC_PACKET_H

#include "BaseTypes.h"

class GenericPacket
{
public:
   
   enum Type
   {
      TypeNotSet
   };
   
   struct __attribute__((__packed__)) Data
   {
   };
   
   GenericPacket();
   
   GenericPacket(const GenericPacket &other);
   
   GenericPacket(ui32 nSizeBytes);
   
   //GenericPacket(unsigned int nHdrSizeBytes, unsigned int nDataSizeBytes);
   
   virtual ~GenericPacket();
   
   bool  allocate();
   
   void  deallocate();
   
   bool  isAllocated() const;
   
   void  destroy();
   
   //ui32 size() const;
      
   /**
    * Returns the the full number of bytes allocated to this packet.
    * As this is a property of the base packet, the method cannot be overriden.
    */
   ui32 allocatedSize() const;
   
   /**
    * This size of the immediate packet's Data definition.
    * All Packet instances must override this method.
    */
   virtual ui32 dataSize() const = 0;
   
   /**
    * Returns the number of bytes that precede this packets Data field.
    */
   virtual ui32 headerSize() const;
   
   /**
    * Returns the sume of the headerSize() and dataSize().
    */
   virtual ui32 headerAndDataSize() const;
   
   //virtual void* const data() const;
   
   virtual bool  pack(void** pData, ui32 &nSizeBytes) const;
   
   virtual bool  unpack(const void* pData, ui32 nSizeBytes);
   
   GenericPacket& operator= (const GenericPacket& other);
   
protected:
   
   //void* hdrPtr();
   
   virtual void*        dataPtr();
   virtual void* const  dataPtr() const;
   
   virtual void*        dataEndPtr() = 0;
   virtual void* const  dataEndPtr() const = 0;
   
protected:
   
   unsigned char* m_pPkt;
   
   //unsigned int   m_nHdrSizeBytes;
   
   ui32   m_nSizeBytes;
};

#endif // GENERIC_PACKET_H