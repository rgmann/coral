#ifndef SERVER_NODE_H
#define SERVER_NODE_H

#include <vector>
#include "Thread.h"
#include "ServerWorker.h"

namespace liber {
namespace netapp {

class ServerNode {
public:
   
  ServerNode();
   
  ~ServerNode();
   
  bool  addWorker(ServerWorker *pWorker);
   
  int   getWorkerCount();
   
  bool  start();
   
  bool  stop();
   
  void  setRecvTimeout(ui32 timeouMs);
   
  float getMeanWorkLat() const;
   
  float getMeanRxLat() const;
   
  float getMeanTxLat() const;
   
  static const ui32 MaxRecvTimeoutMs = 5000;
   
private:
   
  static void rxThreadFunc(ThreadArg* pArg);
   
  void        rxThread(ThreadArg* pArg);
   
  static void txThreadFunc(ThreadArg* pArg);
   
  void        txThread(ThreadArg* pArg);
   
  static void workThreadFunc(ThreadArg* pArg);
   
  void        workThread(ThreadArg* pArg);
   
  void        cleanup();
   
  bool        recvPacket(ServerWorker* pWorker);
   
private:
   
  //std::vector<ServerWorker*> m_vWorkerList;
  Queue<ServerWorker*> mTxQueue;
  Queue<ServerWorker*> mWorkQueue;
  Queue<ServerWorker*> mRxQueue;
   
  Thread *mpRxThread;
   
  Thread *mpWorkerThread;
   
  Thread *mpTxThread;
   
  // ServerWorker receive timeout in milliseconds
  ui32  mnTimeoutMs;
};

}}

#endif // SERVER_NODE_H
