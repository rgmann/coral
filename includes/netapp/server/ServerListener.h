#ifndef  SERVER_LISTENER_H
#define  SERVER_LISTENER_H

#include <vector>
#include "BaseTypes.h"
#include "TcpServer.h"
#include "Thread.h"
#include "Queue.h"
#include "ServerNode.h"
#include "ServerWorker.h"

namespace liber {
namespace netapp {

class ServerListener {
public:

  ServerListener();
   
  virtual ~ServerListener();
   
  void  setPort(ui32 port);
   
  bool  start();
   
  bool  stop();
   
protected:
   
  virtual ServerWorker* createWorker(liber::net::TcpSocket* pSocket) = 0;
   
  bool  distributeWorker(ServerWorker* pWorker);
   
  static void listenThreadFunc(ThreadArg* pArg);
   
  void listenThread(ThreadArg* pArg);
   
  static void distribThreadFunc(ThreadArg* pArg);
   
  void distribThread(ThreadArg* pArg);
   
protected:
   
  ui32        mnServerPort;
   
  Thread*     mpListenerThread;
   
  Thread*     mpBalancerThread;
   
  Queue<ServerWorker*> mWorkerQueue;
   
  ui32        mnMinNodeCount;
   
  std::vector<ServerNode*> mvNodeList;
};

}}

#endif // SERVER_LISTENER_H
