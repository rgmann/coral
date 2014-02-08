#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "TcpSocket.h"

namespace liber {
namespace net {

class TcpClient {
public:
	
   static TcpSocket* Connect(const char* host, int nPort, int nTimeoutMs);

  static bool Connect(TcpSocket&  rSocket,
                      const char* host,
                      int         nPort,
                      int         nTimeoutMs);

private:
	
   TcpClient();
	
   ~TcpClient();
};

}}

#endif // TCP_CLIENT_H

