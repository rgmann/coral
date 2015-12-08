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



#include "Log.h"
#include "GenericPacket.h"
#include "PacketSubscriber.h"
#include "IntraRouter.h"

using namespace coral;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
IntraRouter::IntraRouter()
   : PacketRouter(&mReceiver)
   , IThread("IntraRouter")
   , mpCounterpart(NULL)
{
}

//-----------------------------------------------------------------------------
IntraRouter::~IntraRouter()
{
}

//-----------------------------------------------------------------------------
void IntraRouter::setCounterpart( IntraRouter* pCounterpart )
{
   mpCounterpart = pCounterpart;
}

//-----------------------------------------------------------------------------
void IntraRouter::run(const bool& shutdown)
{
   while ( shutdown == false )
   {
      PacketContainer* container_ptr = NULL;

      if ( ( container_ptr = mReceiver.pop() ) != NULL )
      {
         if ( mpCounterpart )
         {
            mpCounterpart->routePacket( container_ptr );
         }
         else
         {
            log::error("IntraRouter - no counterpart set\n");
         }

         if ( container_ptr->packet_ptr_ )
         {
            delete container_ptr->packet_ptr_;
            container_ptr->packet_ptr_ = NULL;
         }

         delete container_ptr;
      }
   }
}

//-----------------------------------------------------------------------------
void IntraRouter::routePacket( PacketContainer* pContainer )
{
   publish(
      pContainer->destination_id_,
      pContainer->packet_ptr_->basePtr(),
      pContainer->packet_ptr_->allocatedSize());
}


