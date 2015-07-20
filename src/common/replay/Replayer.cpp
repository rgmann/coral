#include "Replayer.h"

using namespace liber::replay;

//-----------------------------------------------------------------------------
Replayer::Replayer( ReplayEventLog& event_log, ReplayEventId last_event_id )
: event_log_ ( event_log )
, last_event_id_ ( last_event_id )
{

}

//-----------------------------------------------------------------------------
bool Replayer::run( ReplayEventDelegate& delegate )
{
  bool replay_success = event_log_.get_replay_iterator(
    last_event_id_,
    iterator
  );

  if ( replay_success )
  {
    for (; iterator != event_log_.end(); ++iterator )
    {
      delegate( *iterator );
      last_event_id_ = ( *iterator )->event_id();
    }
  }

  return replay_success;
}

