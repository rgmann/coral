#ifndef SOCKET_ATTRIBUTES_H
#define SOCKET_ATTRIBUTES_H

#include <netinet/in.h>
#include <sys/select.h>

namespace liber {
namespace net {

struct SocketAttributes {
  int                port;
  int                fd;
  fd_set             set;
  struct sockaddr_in addr;
};

}}

#endif // SOCKET_ATTRIBUTES_H

