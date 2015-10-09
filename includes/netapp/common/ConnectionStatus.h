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



#ifndef CONNECTION_STATUS_H
#define CONNECTION_STATUS_H

#include "Timestamp.h"
#include "GenericPacket.h"
#include "PacketSubscriber.h"

namespace liber {
namespace netapp {


class KeepalivePacket : public GenericPacket {
public:

  struct __attribute__((__packed__)) Data {
    ui32  count;
  };

  KeepalivePacket();

  Data* const data();

  virtual void swap(void* pData, ui32 nSizeBytes);
};


class ConnectionStatus : public PacketSubscriber {
public:

  // Reserver PacketSubscriber ID
  static const int  SubscriberId = 0;
  static const ui32 DefaultConnectionTimeoutMs = 5000;

  ConnectionStatus(ui32 nTimeoutMs = DefaultConnectionTimeoutMs);
  virtual ~ConnectionStatus() {};

  void setReceiveTimeoutMs(ui32 nTimeoutMs);

  /**
   * Returns true if the connection is active.
   * The connection is active if it has not timed out.
   */
  bool isActive() const;

  void sampleRecvTime();

protected:

  Timestamp mLastRecvTime;

  ui32      mnTimeoutMs;
};

}}

#endif // CONNECTION_STATUS_H

