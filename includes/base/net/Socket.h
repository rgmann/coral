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
  virtual void read(SocketStatus& status, char* pData, int nMaxBytes, int nTimeoutMs);

  /**
   *
   */
  virtual void write(SocketStatus& status, const char* pData, int nBytes, int nTimeoutMs = 0);

  static SocketError TranslateErrno(int err);

protected:

  struct SocketAttributes mAttributes;
};

}}

#endif // SOCKET_H

