#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>
#include <sys/select.h>
#include "TcpSocket.h"

namespace liber {
namespace net {

class TcpServer {
public:

  TcpServer();

  ~TcpServer();

  bool start(int nPort);

  void stop();
	
  TcpSocket* acceptClient(int nTimeoutMs);
	
private:

  int mListenSocket;
	
  int mnPort;
	
  fd_set mServerSet;
};

}}

#endif // TCP_SERVER_H
