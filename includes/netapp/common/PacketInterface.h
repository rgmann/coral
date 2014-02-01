
namespace liber {
namespace netapp {

class PacketInterface {
public:

  PacketInterface();
  virtual ~PacketInterface();

  void setId(int nId);

  virtual void sendPacket(NetAppPacket* pPacket);

  virtual void setReceiveQueue(Queue<NetAppPacket*>* pQueue);

protected:

  

protected:

  int mInterfaceId;

  Queue<NetAppPacket*>* pQueue> mpReceiveQueue;

private:

  Thread* mpReceiveThread;

};

