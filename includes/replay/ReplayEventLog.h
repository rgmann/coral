#ifndef  REPLAY_EVENT_LOG_H
#define  REPLAY_EVENT_LOG_H

namespace liber {
namespace replay {

typedef std::list<ReplayEvent*>::iterator ReplayIterator;

class ReplayEventLog {
public:

  ReplayEventLog();
  ~ReplayEventLog();

  bool open( const boost::filesystem::path& replay_log_path, bool load = false );

  bool log_event( ReplayEvent* event_ptr, bool flush = false );

  bool get_replay_iterator( ReplayEventId last_event_id, ReplayIterator& iterator );

  //
  // Flush non-persistent events to disk.
  //
  bool flush();

  bool close();

private:

  bool write_event( ReplayEvent* event_ptr );

  bool read_event( ReplayEvent** event_ptr );

private:

  boost::mutex  lock_;

  ReplayEventId last_assigned_event_id_;

  std::list<ReplayEvent*> replay_events_;

  std::ifstream  read_stream_;

  std::ofstream  append_stream_;
};

}
}

#endif  // REPLAY_EVENT_LOG_H