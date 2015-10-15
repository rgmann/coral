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



#include <boost/date_time/posix_time/posix_time.hpp>
#include "Authority.h"
#include "Log.h"
#include "LocalAuthorityInterface.h"
#include "RsyncJob.h"
#include "FileSystemInterface.h"

#define  DEFAULT_SEGMENT_TIMEOUT_MS     (100000)

using namespace coral;
using namespace coral::rsync;

//----------------------------------------------------------------------------
LocalAuthorityInterface::LocalAuthorityInterface()
: segment_timeout_ms_( DEFAULT_SEGMENT_TIMEOUT_MS )
{
}

//----------------------------------------------------------------------------
LocalAuthorityInterface::~LocalAuthorityInterface()
{
}

//----------------------------------------------------------------------------
std::ifstream& LocalAuthorityInterface::file()
{
  return authoritative_file_;
}

//----------------------------------------------------------------------------
void LocalAuthorityInterface::process( RsyncJob* job_ptr )
{
   processJob( job_ptr, job_ptr->instructions() );
}

//----------------------------------------------------------------------------
void LocalAuthorityInterface::process(
   RsyncJob*             job_ptr,
   InstructionReceiver&  instruction_receiver
)
{
   processJob( job_ptr, instruction_receiver );
}

//----------------------------------------------------------------------------
void LocalAuthorityInterface::processJob(
   RsyncJob*             job_ptr,
   InstructionReceiver&  instruction_receiver
)
{
   RsyncError job_status = hashSegments( job_ptr );

   if ( job_status == kRsyncSuccess )
   {
      // Hash has been populated. Now the Authority can begin building the
      // instructions.
      bool auth_success = job_ptr->fileSystem().open(
         job_ptr->descriptor().getSource().path(),
         file()
      );

      if ( auth_success )
      {
         auth_success = authority_.process(
            job_ptr->descriptor(),
            file(),
            instruction_receiver,
            job_ptr->report().source
         );

         if ( auth_success == false )
         {
            log::error(
               "AuthorityInterface: "
               "Authoritative processing for %s failed.\n",
               job_ptr->descriptor().getSource().path().string().c_str()
            );
         }

         file().close();
      }
      else
      {
         log::error(
           "AuthorityInterface: Failed to open %s\n",
           job_ptr->descriptor().getSource().path().string().c_str()
         );

         job_status = kRsyncSourceFileNotFound;
      }
   }

   if ( job_status != kRsyncSuccess )
   {
      EndInstruction instruction;
      instruction.cancel( job_status );

      instruction_receiver.push( instruction.instruction() );
   }
}

//-----------------------------------------------------------------------------
RsyncError LocalAuthorityInterface::hashSegments( RsyncJob* job_ptr )
{
   RsyncError hash_status = kRsyncSuccess;

   int  received_segment_count = 0;
   bool hash_insert_done       = false;

   job_ptr->report().source.authority.hashBegin.sample();

   while ( hash_insert_done == false )
   {
      Segment* segment_ptr = NULL;

      bool received_segment = job_ptr->segments().pop(
         &segment_ptr,
         segment_timeout_ms_
      );

      if ( received_segment && ( segment_ptr != NULL ) )
      {
         hash_insert_done = segment_ptr->endOfStream();

         if ( hash_insert_done == false )
         {
            received_segment_count++;

            authority_.hash().insert(
               segment_ptr->getWeak().checksum(),
               segment_ptr
            );
         }
      }
      else
      {
         log::error("AuthorityInterface::processJob - "
                    "Timed out waiting for segment (%d).\n",
                    received_segment_count);

         hash_status = kRsyncDestSegmentTimeout;

         // Stop the hash-insert loop as no more segments will be received.
         hash_insert_done = true;
      }
   }

   job_ptr->report().source.authority.hashEnd.sample();

   return hash_status;
}

