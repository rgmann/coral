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

