#ifndef GENERIC_PACKET_H
#define GENERIC_PACKET_H

#include "BaseTypes.h"

namespace liber {
namespace netapp {

class GenericPacket {
public:
   
   enum Type
   {
      TypeNotSet
   };
   
   struct __attribute__((__packed__)) Data
   {
   };
   
   GenericPacket();
   
   explicit GenericPacket(const GenericPacket &other);
   
   explicit GenericPacket(ui32 nSizeBytes);
   GenericPacket(ui32 nDataSizeBytes, ui32 nBodySizeBytes);
   
   virtual ~GenericPacket();
   
   bool  allocate();
   virtual bool  allocate(ui32 sizeBytes);
   virtual bool  allocate(ui32 nDataSizeBytes, ui32 nBodySizeBytes);
   
   void  deallocate();
   
   bool  isAllocated() const;
   
   void  destroy();
         
   /**
    * Returns the the full number of bytes allocated to this packet.
    * As this is a property of the base packet, the method cannot be overriden.
    */
   ui32 allocatedSize() const;
   
   /**
    * This size of the immediate packet's Data definition.
    * All Packet instances must override this method.
    */
   virtual ui32 dataSize() const;
   
   virtual bool  pack(void** pData, ui32 &nSizeBytes) const;
   
   virtual bool  unpack(const void* pData, ui32 nSizeBytes);
   
   GenericPacket& operator= (const GenericPacket& other);
   
   /**
    * Returns a pointer to the very beginning of the packet.  Cannot be
    * overriden.
    */
   void*       basePtr();
   void* const basePtr() const;
   
   virtual void*        dataPtr();
   virtual void* const  dataPtr() const;

   virtual void swapByteOrder();
   virtual void swap(void* pData, ui32 nSizeBytes) = 0;

   void printDump();

protected:
   
   unsigned char* m_pPkt;

   ui32   mnDataSizeBytes;      
   ui32   m_nSizeBytes;
};

}}

#endif // GENERIC_PACKET_H
