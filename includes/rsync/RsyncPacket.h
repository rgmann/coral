#ifndef  RSYNC_PACKET_H
#define  RSYNC_PACKET_H

#include "GenericPacket.h"

//struct __attribute__((__packed__)) RsyncPacketHdr
//{
//   static const ui32  marker   = 0xFEEDBEEF;
//   ui32  type;
//   ui32  length;
//};

class RsyncPacket : public GenericPacket
{
public:
   
   enum Type
   {
      TypeNotSet,
      SegmentReportType,
      AssemblyInstType,
      NumTypes
   };
   
   struct __attribute__((__packed__)) Data
   {
      static const ui32  Marker   = 0xFEEDBEEF;
      ui32  type;
      ui32  length;
   };
   
   RsyncPacket();
   
   RsyncPacket(Type type, ui32 nDataSize);
   
   Type type() const;
   
   //virtual void* const data() const;

   virtual bool  unpack(const void* pPkt, unsigned int nSizeBytes);

protected:
   
   //virtual void* dataPtr();
   
private:
   
};

#endif   // RSYNC_PACKET_H