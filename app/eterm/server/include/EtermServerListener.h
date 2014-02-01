#ifndef ETERM_SERVER_LISTENER_H
#define ETERM_SERVER_LISTENER_H

#include "ServerListener.h"

class EtermServerListener : public liber::netapp::ServerListener {
public:

  EtermServerListener();
  ~EtermServerListener();

protected:

  liber::netapp::ServerWorker* createWorker(liber::net::TcpSocket* pSocket);

};

#endif // RPC_SERVER_LISTENER_H

