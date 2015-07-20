#ifndef  REPLAY_EVENT_H
#define  REPLAY_EVENT_H

#include <boost/date_time/posix_time/posix_time.hpp>

namespace liber {
namespace replay {

typedef ui64 ReplayEventId;

class ReplayEvent : public liber::netapp::Serializable {
public:

  ReplayEvent();
  ~ReplayEvent();

  ui64 event_id();

  const boost::posix_time::ptime& creation_time() const;

  const std::string& data();

  void set_data( const std::string& data );

private:

  ui64 event_id_;

  boost::posix_time::ptime creation_time_;

  bool persistent_;

  std::string data_;

  friend class ReplayEventLog;
};

}
}

#endif // REPLAY_EVENT_H