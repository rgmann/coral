#ifndef RSYNC_PACKET_H
#define RSYNC_PACKET_H

#include <string>
#include <boost/uuid/uuid.hpp>
#include "GenericPacket.h"

namespace liber {
namespace rsync {

class RsyncTransportPacket : public liber::netapp::GenericPacket {
public:

   enum Type {
      kRsyncUnknownDestination = 0,
      kRsyncAuthorityService,
      kRsyncAuthorityInterface,
      kRsyncJobAgent
   };

   struct __attribute ((__packed__)) Data
   {
      ui32 length;
      ui32 type;
      ui8  job_id[ sizeof(boost::uuids::uuid) ];
   };

   RsyncTransportPacket( Type type, const GenericPacket* packet_ptr );

   Data* const data() const;

   void swap(void* data_ptr, ui32 length );

private:

   typedef liber::netapp::GenericPacket inherited;
};


class RsyncPacket : public liber::netapp::GenericPacket {
public:

   enum Type
   {
      RsyncUnknownType,
      RsyncAuthorityService,
      RsyncAuthorityInterface,
      RsyncJobAgent,

      //
      // To RsyncAuthorityService:
      //
      RsyncRemoteAuthQuery,
      RsyncSegment,

      //
      // To RsyncAuthorityInterface:
      //
      RsyncRemoteAuthAcknowledgment,
      RsyncInstruction,
      RsyncAuthorityReport,

      //
      // To RsyncJobAgent:
      //
      RsyncJobRequest,
      RsyncJobComplete
   };

   struct __attribute ((__packed__)) Data
   {
      ui32 length;
      ui32 type;
   };

   RsyncPacket();
   RsyncPacket(int type, ui32 nLength, const void* pData = NULL);
   RsyncPacket(int type, const std::string& rData);
   RsyncPacket(int type, const GenericPacket* pPacket);


   Data* const data() const;

   void swap(void* pData, ui32 nSizeBytes);

   bool unpack(const void* pData, ui32 nSizeBytes);

private:

   typedef liber::netapp::GenericPacket inherited;

};

class RsyncPacketLite {
public:

   RsyncPacketLite( const void* data_ptr, ui32 length );

   bool valid() const;

   const RsyncPacket::Data* header() const;

   const void* data() const;

private:

   bool valid_;
   const void* data_ptr_;
   ui32 length_;
};

}}

#endif // RSYNC_PACKET_H
