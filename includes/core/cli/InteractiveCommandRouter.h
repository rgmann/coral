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

   //
   // Set the message printed at startup.
   //
   void setStartupMessage( const std::string& message );

   //
   // Set the message printed on shutdown (defaults to "bye")
   //
   void setShutdownMessage( const std::string& message );


// private:

   void processLine( std::istream& stream );


private:

   // Command prompt
   std::string prompt_;

   std::string startup_message_;

   std::string shutdown_message_;

   // Map from command/alias name to command object
   typedef std::map<std::string,InteractiveCommand*> CommandTable;
   CommandTable commands_;

   // Quit signal
   bool quit_signalled_;
};

} // namespace cli
} // namespace liber

#endif // INTERACTIVE_COMMAND_ROUTER_H
