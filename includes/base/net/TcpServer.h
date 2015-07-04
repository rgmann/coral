#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>
#include <sys/select.h>

namespace liber {
namespace net {

class TcpSocket;

class TcpServer {
public:

  TcpServer();

  ~TcpServer();

  bool start(int nPort);

  void stop();
	
  TcpSocket* acceptClient(int nTimeoutMs);
  bool acceptClient(TcpSocket& rSocket, int nTimeoutMs);
	
private:

  int mListenSocket;
	
  int mnPort;
	
  fd_set mServerSet;
};

}}

#endif // TCP_SERVER_H

