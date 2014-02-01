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
class RsyncClient : public liber::netapp::PacketSubscriber {
public:

  Rsync();
  ~Rsync();

  void pause();
  void resume();

  void setCompletionCallback(RsyncJobCallback* pCallback);

  bool push(RsyncJob& job,
            const liber::fs::FilePath& source,
            const liber::fs::FilePath& destination);

  bool pull(RsyncJob& job,
            const liber::fs::FilePath& source,
            const liber::fs::FilePath& destination);

  bool put(const char* pData, ui32 nLength);

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

