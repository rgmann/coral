#ifndef  RSYNC_QUERY_RESPONSE_H
#define  RSYNC_QUERY_RESPONSE_H

#include <boost/uuid/uuid.hpp>
#include "Serializable.h"
#include "RsyncError.h"

namespace liber {
namespace rsync {

class RsyncQueryResponse : public liber::netapp::Serializable {
public:

  RsyncQueryResponse();
  RsyncQueryResponse( const boost::uuids::uuid& uuid, RsyncError status );

  const boost::uuids::uuid& uuid() const;

  RsyncError status() const;

private:

  void pack(liber::netapp::SerialStream& rCtor );
  void pack(liber::netapp::SerialStream& rCtor ) const;
  bool unpack(liber::netapp::SerialStream& rDtor);

private:

  boost::uuids::uuid uuid_;

  RsyncError status_;
};

} // end namepsace rsync
} // end namespace liber

#endif // RsyncQueryResponse