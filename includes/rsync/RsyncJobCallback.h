#ifndef  RSYNC_JOB_CALLBACK_H
#define  RSYNC_JOB_CALLBACK_H

namespace liber {
namespace rsync {

class JobDescriptor;
class JobReport;

class RsyncJobCallback {
public:

  RsyncJobCallback() {};
  virtual ~RsyncJobCallback() {};

  virtual void call( const JobDescriptor& job, const JobReport& report ) = 0;

};

}
}

#endif // RSYNC_JOB_CALLBACK_H
