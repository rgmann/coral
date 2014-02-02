#ifndef RSYNC_CLIENT_H
#define RSYNC_CLIENT_H

namespace liber {
namespace rsync {

class RsyncJobCallback : public liber::Callback {
public:

  virtual ~RsyncJobCallback();

  using liber::Callback::call;
  virtual void* call(const liber::rsync::RsyncJobInfo& info) {};

};


// Active Object pattern
// - Packets are routed via the RsyncNode to Authorities
// - Requesters either have the same Router as the server or
//   they are registered with a different router based on which
//   foreign node they are syncing with.
class RsyncNode : public netapp::PacketSubscriber {
public:

  RsyncNode(netapp::PacketRouter& rRouter);
  ~RsyncNode();

  void pause();
  void resume();

  void setCompletionCallback(RsyncJobCallback* pCallback);

  bool pull(netapp::PacketRouter& rRouter,
            RsyncJob& job,
            const liber::fs::FilePath& source);


private:

  static void TranslateThreadEntry(ThreadArg* pArg);
  void translationThread(ThreadArg* pArg);

  static void RsyncJobThreadEntry(ThreadArg* pArg);
  void rsyncJobThread(ThreadArg* pArg);

  static void CallbackThreadEntry(ThreadArg* pArg);
  void callbackThread(ThreadArg* pArg);

private:

  RsyncJobCallback* mpCompletionCallback;

  Queue<RsyncCoreJob*> mPendingJobs;
  Queue<RsyncCoreJob*> mCompletedJobs;

  Queue<RscynPacket*>  mRsyncOutQueue;

  Thread* mpJobThread;
  Thread* mpTranslationThread;
};

#endif // RSYNC_CLIENT_H

