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

  bool send( PacketContainer* pContainer );
  PacketContainer* pop(int nTimeoutMs = liber::thread::Semaphore::SemWaitForever);

private:

  Queue<PacketContainer*> mQueue;

};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_QUEUE_H
