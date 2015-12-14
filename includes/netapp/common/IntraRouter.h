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



#ifndef INTRA_ROUTER_H
#define INTRA_ROUTER_H

#include "IThread.h"
#include "PacketRouter.h"
#include "PacketQueue.h"

namespace coral {
namespace netapp {

///
/// The IntraRouter is useful for testing netapp-based applications without
/// having to setting up the rest of network infrastructure (e.g. loop-back
/// network interface).
///
class IntraRouter : public PacketRouter, public coral::thread::IThread {
public:

   IntraRouter();
   ~IntraRouter();

   ///
   /// Set the IntraRouter's counterpart instance.
   ///
   /// @param  counterpart_ptr  Pointer to counterpart IntraRouter instance.
   /// @return void
   ///
   void setCounterpart( IntraRouter* counterpart_ptr );


protected:

   ///
   /// Publish a packet to all subscribers.
   ///
   /// @param  container_ptr  Packet container to route
   /// @return void
   ///
   void routePacket( PacketContainer* container_ptr );

   ///
   /// Routing thread loop
   ///
   /// @param  shutdown  Thread loop shutdown flag
   /// @return void
   ///
   void run( const bool& shutdown );


private:

   ///
   /// IntraRouter cannot be copied
   ///
   IntraRouter( const IntraRouter& );
   IntraRouter& operator= ( const IntraRouter& );

private:

   IntraRouter* mpCounterpart;

   PacketQueue mReceiver;
};

} // End namespace netapp
} // End namespace coral

#endif // INTRA_ROUTE_H
