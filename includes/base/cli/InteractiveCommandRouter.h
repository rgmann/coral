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

   //
   // Create an interactive command.
   //
   InteractiveCommandRouter( const std::string& prompt = ">" );
   ~InteractiveCommandRouter();

   //
   // Run the interactive command router.
   //
   void run();

   //
   // Register a command with the router. The command name cannot be among the
   // the protected, including 'list', 'guit', and 'exit'.
   //
   bool add( InteractiveCommand* command_ptr );

   //
   // Remove a command by name.
   //
   InteractiveCommand* remove( const std::string& command );


private:

   // Command prompt
   std::string prompt_;

   // Map from command/alias name to command object
   std::map<std::string, InteractiveCommand*> commands_;

   // Quit signal
   bool quit_signalled_;
};

} // namespace cli
} // namespace liber

#endif // INTERACTIVE_COMMAND_ROUTER_H
