#include <utility>
#include <iostream>
#include <iomanip>
#include "InteractiveCommandRouter.h"
#include "StringHelper.h"

using namespace liber::cli;

class QuitCommand : public InteractiveCommand {
public:

  QuitCommand(bool& rQuitSignal)
  : liber::cli::InteractiveCommand("quit", "Exit the terminal", "exit")
  , mrQuitSignal(rQuitSignal)
  {
  };

  ~QuitCommand() {};

  void process(const liber::cli::ArgumentList& args)
  {
    mrQuitSignal = true;
    std::cout << "bye" << std::endl;
  };

private:

  bool& mrQuitSignal;
};

class ListCommand : public InteractiveCommand {
public:

  ListCommand(const std::map<std::string, InteractiveCommand*>& rCommandMap)
  : liber::cli::InteractiveCommand("list", "List supported commands")
  , mrCommandMap(rCommandMap)
  {
  };

  ~ListCommand() {};

  void process(const liber::cli::ArgumentList& args)
  {
    std::map<std::string, InteractiveCommand*>::const_iterator lIt;

    std::cout << "Command List:" << std::endl;
    for (lIt = mrCommandMap.begin(); lIt != mrCommandMap.end(); ++lIt)
    {
      if (lIt->first != lIt->second->alias())
      {
        std::cout << "  ";

        if (lIt->second->hasAlias())
        {
          std::cout << std::left << lIt->first << "/"
                    << std::setw(16 - lIt->first.length() - 1)
                    << std::left << lIt->second->alias();
        }
        else
        {
          std::cout << std::setw(16) << std::left << lIt->first;
        }

        std::cout << "   "
                  << lIt->second->brief()
                  << std::endl;
      }
    }
  };

private:

  const std::map<std::string, InteractiveCommand*>& mrCommandMap;
};

//-----------------------------------------------------------------------------
InteractiveCommandRouter::InteractiveCommandRouter()
: mLineDelimeter(">")
, mbQuitSignalled(false)
{
  add(new ListCommand(mCommands));
  add(new QuitCommand(mbQuitSignalled));
}

//-----------------------------------------------------------------------------
InteractiveCommandRouter::~InteractiveCommandRouter()
{
  InteractiveCommand* lpCommand = NULL;

  lpCommand = remove("list");
  if (lpCommand)
  {
    delete lpCommand;
    lpCommand = NULL;
  }

  lpCommand = remove("quit");
  if (lpCommand)
  {
    delete lpCommand;
    lpCommand = NULL;
  }
}

//-----------------------------------------------------------------------------
bool InteractiveCommandRouter::add(InteractiveCommand* pCommand)
{
  bool lbSuccess = false;

  if (pCommand)
  {
    lbSuccess = (mCommands.count(pCommand->command()) == 0);

    if (pCommand->hasAlias())
    {
      lbSuccess &= (mCommands.count(pCommand->alias()) == 0);
    }

    if (lbSuccess)
    {
      mCommands.insert(std::make_pair(pCommand->command(), pCommand));
      mCommands.insert(std::make_pair(pCommand->alias(), pCommand));
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
InteractiveCommand* InteractiveCommandRouter::
remove(const std::string& command)
{
  InteractiveCommand* lpCommand = NULL;

  if (mCommands.count(command) > 0)
  {
    lpCommand = mCommands.find(command)->second;
    
    // Remove the main command.
    mCommands.erase(lpCommand->command());

    // If the command has an alias, remove the alias.
    if (lpCommand->hasAlias())
    {
      mCommands.erase(lpCommand->alias());
    }
  }

  return lpCommand;
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::run()
{
  std::string lCommand;

  while (!mbQuitSignalled)
  {
    std::cout << mLineDelimeter << " ";

    std::getline(std::cin, lCommand);
    if (!std::cin.fail() && lCommand.size() > 0)
    {
      // Split the command string on spaces.
      std::vector<std::string> lvArgs = StringHelper::Split(lCommand, ' ');

      // The command name is always the first token.
      // The remaining tokens are arguments to the command.
      std::string lCommandName = lvArgs.front();
      lvArgs.erase(lvArgs.begin());

      if (mCommands.count(lCommandName) > 0)
      {
        if (mCommands.find(lCommandName)->second)
        {
          mCommands.find(lCommandName)->second->process(lvArgs);
        }
      }
      else
      {
        std::cout << "Error: Unknown command \""
                  << lCommandName << "\"." << std::endl;
      }
    }
  }
}


