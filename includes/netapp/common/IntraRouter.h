#ifndef INTRA_ROUTER_H
#define INTRA_ROUTER_H

#include "IThread.h"
#include "PacketRouter.h"
#include "PacketQueue.h"

namespace liber {
namespace netapp {

class IntraRouter : public PacketRouter, public liber::concurrency::IThread {
public:

  IntraRouter();
  ~IntraRouter();

  void setCounterpart(IntraRouter* pCounterpart);

protected:

  void routePacket(PacketContainer* pContainer);

  void run(const bool& bShutdown);

private:

  IntraRouter* mpCounterpart;
  PacketQueue mReceiver;
};

} // End namespace netapp
} // End namespace liber

#endif // INTRA_ROUTE_H
