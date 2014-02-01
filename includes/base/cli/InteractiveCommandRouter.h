#ifndef INTERACTIVE_COMMAND_ROUTER_H
#define INTERACTIVE_COMMAND_ROUTER_H

#include <map>
#include <string>
#include "BaseTypes.h"
#include "InteractiveCommand.h"

namespace liber {
namespace cli {

class InteractiveCommandRouter {
public:

  InteractiveCommandRouter();
  ~InteractiveCommandRouter();

  void run();

  void setLineDelimeter(const std::string& lineDelimeter);

  bool add(InteractiveCommand* pCommand);

  InteractiveCommand* remove(const std::string& command);

private:

  std::string mLineDelimeter;
  std::map<std::string, InteractiveCommand*> mCommands;

  bool mbQuitSignalled;
};

}}

#endif // INTERACTIVE_COMMAND_ROUTER_H
