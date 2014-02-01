
namespace liber {
namespace rsync {

class RsyncPushJob : public liber:rsync::RsyncBaseJob {
public:

  RsyncPushJob(const std::string& filepath);
  ~RsyncPushJob();

  bool fileExists();

private:

  RsyncFileAuthority mAuthority;

};

}}

