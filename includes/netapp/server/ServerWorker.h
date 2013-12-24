#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include "BaseTypes.h"
#include "GenericPacket.h"
#include "Mutex.h"
#include "Queue.h"
#include "TcpSocket.h"
#include "Timestamp.h"
#include "ServerPacketRouter.h"

namespace liber {
namespace netapp {

class ServerWorker {
public:
   
  ServerWorker(liber::net::TcpSocket* pSocket);
   
  virtual ~ServerWorker();
   
  bool  initialize();
   
  liber::net::TcpSocket* socket();
  netapp::ServerPacketRouter& router();   
//  bool  lockSocket(ui32 timeoutMs);
   
//  void  releaseSocket();
   
  ui32   elapseMsSinceRecv();
   
  virtual bool put(liber::netapp::NetAppPacket* pPacket);
   
  virtual void work(int nSlotTimeUs);
   
  virtual netapp::NetAppPacket* get();


protected:
   
  void  sampleRecvTime();
   
  /*void  pushRx(liber::netapp::NetAppPacket*);
  bool  popRx(liber::netapp::NetAppPacket**);
   
  void  pushTx(liber::netapp::NetAppPacket*);
  bool  popTx(liber::netapp::NetAppPacket**);
   */
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
   
  Timestamp   mRecvTs;
};

}}

#endif // SERVER_WORKER_H
