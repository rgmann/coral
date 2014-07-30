#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include "Queue.h"
#include "PacketReceiver.h"

namespace liber {
namespace netapp {

class PacketQueue : public PacketReceiver {
public:

  PacketQueue();
  virtual ~PacketQueue();

  bool push(PacketContainer* pContainer);
  PacketContainer* pop(int nTimeoutMs = Sem::SemWaitForever);

private:

  Queue<PacketContainer*> mQueue;

};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_QUEUE_H
