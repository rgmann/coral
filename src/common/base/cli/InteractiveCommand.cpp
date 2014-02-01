#include "InteractiveCommand.h"

using namespace liber::cli;

//-----------------------------------------------------------------------------
InteractiveCommand::
InteractiveCommand(const std::string& command,
                   const std::string& brief,
                   const std::string& alias)
: mCommand(command)
, mBrief(brief)
, mAlias(alias)
{
}

//-----------------------------------------------------------------------------
InteractiveCommand::~InteractiveCommand() {}

//-----------------------------------------------------------------------------
const std::string& InteractiveCommand::command() const
{
  return mCommand;
}

//-----------------------------------------------------------------------------
const std::string& InteractiveCommand::alias() const
{
  return mAlias;
}

//-----------------------------------------------------------------------------
bool InteractiveCommand::hasAlias() const
{
  return (mAlias.empty() == false);
}

//-----------------------------------------------------------------------------
std::string& InteractiveCommand::brief()
{
  return mBrief;
}

