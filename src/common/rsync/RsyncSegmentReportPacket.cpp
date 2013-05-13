#include <stdio.h>
#include "RsyncSegmentReportPacket.h"

//------------------------------------------------------------------------------
RsyncSegmentReportPacket
::RsyncSegmentReportPacket()
: RsyncPacket(SegmentReportType, sizeof(Data))
{
   //m_nHdrSizeBytes = sizeof(Data);
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket
::RsyncSegmentReportPacket(Type type, ui32 nDataSize)
: RsyncPacket(SegmentReportType, sizeof(Data) + nDataSize)
{
//   Data* l_pData = NULL;
   
   if (dataPtr())
   {
//      l_pData = reinterpret_cast<RsyncSegmentReportPacketHdr*>(dataPtr());
//      l_pData = reinterpret_cast<Data*>(dataPtr());
//      Data* const l_pData = reinterpret_cast<Data* const>(data());
//      l_pData->type = type;
//      l_pData->length = nDataSize;
      headerData()->type = type;
      headerData()->length = nDataSize;
   }
}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::Type RsyncSegmentReportPacket::type() const
{
   Type l_type = TypeNotSet;
   
   if (headerData())
   {
      //l_pData = reinterpret_cast<const Data*>(dataPtr());
      l_type = (Type)headerData()->type;//(Type)l_pData->type;
   }
   
   return l_type;
}

//------------------------------------------------------------------------------
//void* RsyncSegmentReportPacket::dataPtr()
//{
//   ui8*  l_pData = NULL;
//   
//   if (RsyncPacket::dataPtr())
//   {
//      l_pData = reinterpret_cast<ui8*>(RsyncPacket::dataPtr());
//      //l_pData += sizeof(RsyncSegmentReportPacket::Data);
//   }
//   
//   return reinterpret_cast<void*>(l_pData);
//}

//------------------------------------------------------------------------------
//void* const RsyncSegmentReportPacket::data() const
//{   
//   if (RsyncPacket::data())
//   {
//      return reinterpret_cast<void* const>(
//                        reinterpret_cast<ui8* const>(RsyncPacket::data()));
//   }
//   
//   return NULL;
//}

//------------------------------------------------------------------------------
RsyncSegmentReportPacket::Data* RsyncSegmentReportPacket::headerData()
{
   return reinterpret_cast<Data*>(dataPtr());
}

//------------------------------------------------------------------------------
const RsyncSegmentReportPacket::
Data* RsyncSegmentReportPacket::headerData() const
{
   return reinterpret_cast<const Data*>(data());
}
