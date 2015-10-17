// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 


#ifndef INTERACTIVE_COMMAND_ROUTER_H
#define INTERACTIVE_COMMAND_ROUTER_H

#include <map>
#include <string>
#include "BaseTypes.h"
#include "InteractiveCommand.h"

namespace coral {
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
} // namespace coral

#endif // INTERACTIVE_COMMAND_ROUTER_H
