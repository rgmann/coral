#include "Log.h"
#include "RsyncQueryResponse.h"

using namespace liber::netapp;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncQueryResponse::RsyncQueryResponse()
:  status_( kRsyncCommError )
{
}

//-----------------------------------------------------------------------------
RsyncQueryResponse::RsyncQueryResponse(
  const boost::uuids::uuid& uuid,
  RsyncError               status )
: uuid_( uuid )
, status_( status )
{
}

//-----------------------------------------------------------------------------
const boost::uuids::uuid& RsyncQueryResponse::uuid() const
{
  return uuid_;
}

//-----------------------------------------------------------------------------
RsyncError RsyncQueryResponse::status() const
{
  return status_;
}

//-----------------------------------------------------------------------------
void RsyncQueryResponse::pack(liber::netapp::SerialStream& ctor)
{
  const_cast<const RsyncQueryResponse*>(this)->pack(ctor);
}

//-----------------------------------------------------------------------------
void RsyncQueryResponse::pack(liber::netapp::SerialStream& ctor) const
{
  ctor.write( (ui32)status_ );
  ctor.write((const char*)uuid_.data, uuid_.size());
}

//-----------------------------------------------------------------------------
bool RsyncQueryResponse::unpack(liber::netapp::SerialStream& dtor)
{
  ui32 temp_status = kRsyncCommError;
  if ( dtor.read( temp_status ) == false )
  {
    liber::log::error(
      "RsyncQueryResponse::unpack: Failed to deserialize segment size.\n");
    return false;
  }
  status_ = (RsyncError)temp_status;

  if ( dtor.read( (char*)uuid_.data, uuid_.size() ) != SerialStream::ReadOk )
  {
    liber::log::error( "RsyncQueryResponse::unpack - failed to deserialize UUID\n" );
    return false;
  }

  return true;
}
