#ifndef INTRA_ROUTER_H
#define INTRA_ROUTER_H

#include "IThread.h"
#include "PacketRouter.h"

namespace liber {
namespace netapp {

class IntraRouter
: public PacketRouter
, public liber::concurrency::IThread {
public:

  IntraRouter();
  ~IntraRouter();

  void setCounterpart(IntraRouter* pCounterpart);

protected:

  void routePacket(NetAppPacket* pPacket);

  void run(const bool& bShutdown);

private:

  IntraRouter* mpCounterpart;
  Queue<NetAppPacket*> mOutQueue;
};

} // End namespace netapp
} // End namespace liber

#endif // INTRA_ROUTE_H
