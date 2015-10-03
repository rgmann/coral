#include "InteractiveCommand.h"

using namespace liber::cli;

//-----------------------------------------------------------------------------
InteractiveCommand::InteractiveCommand(
  const std::string& command,
  const std::string& brief,
  const std::string& alias
)
: command_( command )
, brief_( brief )
, alias_( alias )
{
}

//-----------------------------------------------------------------------------
InteractiveCommand::~InteractiveCommand() {}

//-----------------------------------------------------------------------------
const std::string& InteractiveCommand::command() const
{
  return command_;
}

//-----------------------------------------------------------------------------
const std::string& InteractiveCommand::alias() const
{
  return alias_;
}

//-----------------------------------------------------------------------------
bool InteractiveCommand::hasAlias() const
{
  return ( alias_.empty() == false );
}

//-----------------------------------------------------------------------------
std::string& InteractiveCommand::brief()
{
  return brief_;
}

