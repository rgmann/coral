#ifndef REMOTE_JOB_RESULT_H
#define REMOTE_JOB_RESULT_H

#include <boost/uuid/uuid.hpp>
#include "PacketHelper.h"
#include "JobReport.h"

namespace liber {
namespace rsync {

class RemoteJobResult : public liber::netapp::Serializable {
public:

  RemoteJobResult();
  RemoteJobResult(const boost::uuids::uuid& rID, const JobReport& rReport);

  boost::uuids::uuid& uuid();

  JobReport& report();

protected:

  void pack(liber::netapp::PacketCtor&);
  void pack(liber::netapp::PacketCtor&) const;
  bool unpack(liber::netapp::PacketDtor&);

private:

  boost::uuids::uuid mUUID;
  JobReport mReport;

};

} // End namespace rsync
} // End namespace liber

#endif // REMOTE_JOB_RESULT_H
