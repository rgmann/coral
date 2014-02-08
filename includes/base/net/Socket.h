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

  bool isConnected() const;

  static const int Block = -1;

  /**
   *
   */
  virtual void read(SocketStatus& status, char* pData, int nMaxBytes, int nTimeoutMs = Block);

  /**
   *
   */
  virtual void write(SocketStatus& status, const char* pData, int nBytes, int nTimeoutMs = 0);

  virtual void disconnect();

  static SocketError TranslateErrno(int err);

protected:

  struct SocketAttributes mAttributes;
};

}}

#endif // SOCKET_H

