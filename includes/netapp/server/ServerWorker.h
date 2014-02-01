#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include "BaseTypes.h"
#include "GenericPacket.h"
#include "Mutex.h"
#include "Queue.h"
#include "TcpSocket.h"
#include "Timestamp.h"
#include "ServerPacketRouter.h"
#include "ServerConnectionStatus.h"

namespace liber {
namespace netapp {

class ServerWorker {
public:
   
  ServerWorker(liber::net::TcpSocket* pSocket);
   
  virtual ~ServerWorker();
   
  bool initialize();
  void shutdown();
   
  liber::net::TcpSocket* socket();
  netapp::ServerPacketRouter& router();   
   
  //ui32   elapseMsSinceRecv();
   
  virtual bool put(liber::netapp::NetAppPacket* pPacket);
   
  virtual void work(int nSlotTimeUs);
   
  virtual netapp::NetAppPacket* get();

  const ConnectionStatus& status();

protected:
   
  //void  sampleRecvTime();

  virtual bool setup();
  virtual void teardown();   

protected:
   
  Mutex mSocketLock;
   
  liber::net::TcpSocket*  mpSocket;

  netapp::ServerPacketRouter mRouter;

  static const i32 DefaultEnqueueTimeoutMs = 100;
  i32 mEnqueueTimeoutMs;

  static const i32 DefaultDequeueTimeoutMs = 100;
  i32 mDequeueTimeoutMs;

   // ProcessMsg parses and validates the received packet.
   // If the packet is valid it adds it to the IN queue;
  Queue<liber::netapp::NetAppPacket*> mInQueue;
   
   // The work method adds packed messages to the output queue.
  Queue<liber::netapp::NetAppPacket*> mOutQueue;
   
  //Timestamp   mRecvTs;
  ServerConnectionStatus mConnectionStatus;
};

}}

#endif // SERVER_WORKER_H
