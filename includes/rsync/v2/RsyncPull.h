
namespace liber {
namespace rsync {

class RsyncPullJob : public liber::rsync::RsyncBaseJob {
public:

  bool segmentFile(Queue<RsyncPacket*> pOutQueue);

};

}}

