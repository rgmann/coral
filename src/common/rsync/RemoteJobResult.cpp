#include <boost/uuid/nil_generator.hpp>
#include "Log.h"
#include "RemoteJobResult.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RemoteJobResult::RemoteJobResult()
: Serializable()
{
  mUUID = boost::uuids::nil_uuid();
}

//----------------------------------------------------------------------------
RemoteJobResult::RemoteJobResult(const boost::uuids::uuid& rID, const JobReport& rReport)
{
  mUUID = rID;
  mReport = rReport;
}

//----------------------------------------------------------------------------
boost::uuids::uuid& RemoteJobResult::uuid()
{
  return mUUID;
}

//----------------------------------------------------------------------------
JobReport& RemoteJobResult::report()
{
  return mReport;
}

//----------------------------------------------------------------------------
void RemoteJobResult::pack(PacketCtor& ctor)
{
  const_cast<const RemoteJobResult*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void RemoteJobResult::pack(PacketCtor& ctor) const
{
  ctor.write((const char*)mUUID.data, mUUID.size());
  mReport.serialize(ctor);
}

//----------------------------------------------------------------------------
bool RemoteJobResult::unpack(PacketDtor& dtor)
{
  if (dtor.read((char*)mUUID.data, mUUID.size()) != liber::netapp::PacketDtor::ReadOk)
  {
    log::error("RemoteJobStatus::unpack - failed to deserialize UUID\n");
    return false;
  }

  if (mReport.deserialize(dtor) == false)
  {
    return false;
  }

  return true;
}


