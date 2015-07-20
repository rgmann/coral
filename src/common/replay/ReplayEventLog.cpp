#include "ReplayEventLog.h"

using namespace liber::replay;

//-----------------------------------------------------------------------------
ReplayEventLog::ReplayEventLog()
: last_assigned_event_id_ ( 0 )
{
}

//-----------------------------------------------------------------------------
bool ReplayEventLog::open( const boost::filesystem::path& path )
{
  bool open_success = false;
  boost::system::error_code file_sys_error;

  if ( boost::filesystem::exists( path, file_sys_error ) )
  {
    close();

    write_stream_.open( path.string().c_str(), std::ofstream::app | std::ofstream::binary );
    read_stream_.open( path.string().c_str(), std::ofstream::binary );

    if ( write_stream_.fail() )
    {
      close();
    }
    else if ( read_stream_.fail() )
    {
      close();
    }
    else
    {
      open_success = true;

      if ( load )
      {
        while ( read_stream_.eof() == false )
        {
          ReplayEvent* replay_event_ptr = NULL;

          if ( read_event( &replay_event_ptr ) )
          {
            replay_events_.push_back( replay_event_ptr );
          }
        }
      }
    }
  }

  return open_success;
}

//-----------------------------------------------------------------------------
bool ReplayEventLog::log_event( ReplayEvent* event_ptr, bool flush )
{
  bool log_success = false;

  if ( replay_event_ptr )
  {
    replay_event_ptr->event_id_ = ++last_assigned_event_id_;
    replay_event_ptr->creation_time_ =
      boost::posix_time::microsec_clock::universal_clock();

    log_success = replay_events_.push_back( replay_event_ptr );

    if ( flush )
    {
      flush();
    }
  }

  return log_success;
}

//-----------------------------------------------------------------------------
bool ReplayEventLog::get_replay_iterator(
  ReplayEventId last_event_id,
  ReplayIterator& iterator
)
{
  iterator = replay_events_.begin();
  for (; iterator != replay_events_.end(); ++iterator )
  {
    if ( ( *iterator )->event_id() > last_event_id )
    {
      break;
    }
  }

  return ( iterator != replay_events_.end() );
}

//-----------------------------------------------------------------------------
bool ReplayEventLog::flush()
{
  bool flush_success = false;

  ReplayIterator event_iterator = replay_events_.begin();
  for (; event_iterator != replay_events_.end(); ++event_iterator )
  {
    if ( ( *event_iterator )->persistent_ == false )
    {
      if ( ( flush_success = write_event( *event_iterator ) ) )
      {
        ( *event_iterator )->persistent_ = true;
      }
    }
  }

  return flush_success;
}


//-----------------------------------------------------------------------------
void ReplayEventLog::close()
{
  if ( read_stream_.is_open() ) read_stream_.close();
  if ( write_stream_.is_open() ) write_stream_.close();
}

//-----------------------------------------------------------------------------
bool ReplayEventLog::read_event( ReplayEvent** replay_event_ptr )
{
  bool read_success = false;
  ui32 event_length_bytes = 0;

  read_stream_.read(
    (char*)&event_length_bytes, sizeof( event_length_bytes ) );
  
  if ( read_stream_.gcount() == sizeof( event_length_bytes ) )
  {
    char* data_ptr = new char[ event_length_bytes ];

    read_stream_.read( (char*)data_ptr, event_length_bytes );

    if ( read_stream_.gcount() == event_length_bytes )
    {
      *replay_event_ptr = new ReplayEvent();

      read_success = ( *replay_event_ptr )->deserialize(
        data_ptr, event_length_bytes );

      if ( read_success == false )
      {
        delete *replay_event_ptr;
        *replay_event_ptr = NULL;
      }
    }

    delete[] data_ptr;
  }

  return read_status;
}


//-----------------------------------------------------------------------------
bool ReplayEventLog::write_event( const ReplayEvent* replay_event_ptr )
{
  bool write_success = false;
  ui32 event_length_bytes = 0;

  std::string serialized_event = replay_event_ptr->serialize();

  write_stream_.write(
    (char*)&event_length_bytes, sizeof( event_length_bytes ) );

  if ( write_stream_.gcount() == event_length_bytes )
  {
    write_stream_.write(
      (char*)serialized_event.data(), serialized_event.size() );

    write_success = ( write_stream_.gcount() == event_length_bytes );
  }

  return write_success;
}


