#ifndef  REPLAY_H
#define  REPLAY_H

namespace liber {
namespace replay {

class ReplayEventDelegate {
public:

  virtual ~ReplayEventDelegate(){};

  virtual bool operator() ( const ReplayEvent& event ) = 0;

};

class Replayer {
public:

  Replayer( ReplayEventLog& log, ReplayEventId last_event_id );

  bool run( ReplayEventDelegate& delegate );


private:

  ReplayEventLog& event_log_;

  ReplayEventId last_event_id_;

};

}
}

#endif // REPLAY_H