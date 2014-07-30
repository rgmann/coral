#ifndef APPLICATION_WORKER_H
#define APPLICATION_WORKER_H

#include "Mutex.h"
#include "TcpSocket.h"
#include "PacketQueue.h"
#include "ServerConnectionStatus.h"
#include "ServerPacketRouter.h"

namespace liber {
namespace netapp {

class NetAppPacket;

class ApplicationWorker {
public:

  ApplicationWorker();
  virtual ~ApplicationWorker();

  bool initialize(liber::net::TcpSocket* pSocket);
  void destroy();

  /**
   * Attempt to receive a packet from the socket.
   * Return false if the socket is already disconnected or a socket error
   * occurs. Return true otherwise.
   *
   * If receivePacket() returns false, the socket has already been
   * disconnected.
   */
  bool readPacket(int nTimeoutMs, bool bRouteOnSuccess = true);

  /**
   * Send one packet if a packet is available. The worker is not allowed to
   * block on the outbound queue since it will be allowed another chance to
   * send in short order.
   *
   * Returns false if the socket is already disconnected or if a socket
   * error occurs.  If sendPacket() returns false, the socket has already been
   * disconnected.
   */
  bool writePacket(int nTimeoutMs);

  /**
   * If a packet is available in the input queue, the packet is routed to the
   * destination subscriber. The desination packet subsriber may, or may not,
   * process the packet within the context of the calling thread. It is the
   * responsibility of the subscriber to handle the packet is such a way
   * that it will not block the calling thread for an unreasonable length of
   * time.
   */
  void routePacket();

  const ConnectionStatus& status() const;

  /**
   * Disconnect the socket. Note: It is very possible this will send interrupt
   * or broken-pipe signal to other threads that are concurrently using the
   * socket, so SIGPIPE should be ignored (handled as an error) as should
   * SIGINT. The disconnect is atomic.
   */
  void disconnect();

  liber::net::TcpSocket* const socket();

  ServerPacketRouter& router();

protected:

  virtual bool derivedInitialize();

  virtual void derivedDestroy();

private:

  liber::net::TcpSocket* mpSocket;

  Mutex mSocketLock;
  i32 mnLockTimeoutMs;

  ServerPacketRouter mRouter;

  static const i32 DefaultEnqueueTimeoutMs = 100;
  i32 mEnqueueTimeoutMs;

  static const i32 DefaultDequeueTimeoutMs = 100;
  //i32 mDequeueTimeoutMs;

  // ProcessMsg parses and validates the received packet.
  // If the packet is valid it adds it to the IN queue;
  Queue<NetAppPacket*> mInQueue;

  // The work method adds packed messages to the output queue.
  PacketQueue mOutQueue;

  ServerConnectionStatus mConnectionStatus;
};

} // End namespace netapp
} // End namespace liber

#endif // APPLICATION_WORKER_H

