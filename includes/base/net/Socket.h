#ifndef SOCKET_H
#define SOCKET_H

#include "SocketAttributes.h"

namespace liber {
namespace net {

class Socket {
public:

  Socket();
  virtual ~Socket();

  void setAttributes(const SocketAttributes& attributes);

  /**
   *
   */
  virtual int read(char* pData, int nMaxBytes, int nTimeoutMs);

  /**
   *
   */
  virtual int write(const char* pData, int nBytes, int nTimeoutMs = 0);

  void disconnect();

protected:

  struct SocketAttributes mAttributes;
};

}}

#endif // SOCKET_H

