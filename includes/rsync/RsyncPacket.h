// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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
   };

   RsyncTransportPacket( int type, const GenericPacket* packet_ptr );

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
      ui8  job_id[ 16 ];
      ui32 length;
      ui32 type;
   };

   RsyncPacket();
   RsyncPacket(int type, ui32 nLength, const void* pData = NULL);
   RsyncPacket(int type, const std::string& rData);

   Data* const data() const;

   void swap(void* pData, ui32 nSizeBytes);

private:

   typedef liber::netapp::GenericPacket inherited;

};

class RsyncPacketLite {
public:

   struct __attribute ((__packed__)) Data
   {
      ui32 length;
      ui32 type;
   };

   RsyncPacketLite( const void* data_ptr, ui32 length );

   bool valid() const;

   const Data* header() const;

   const void* data() const;

private:

   bool valid_;
   const void* data_ptr_;
   ui32 length_;
};

}}

#endif // RSYNC_PACKET_H
