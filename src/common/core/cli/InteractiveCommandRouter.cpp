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



#include <utility>
#include <iostream>
#include <iomanip>
#include <memory>
#include "Log.h"
#include "InteractiveCommandRouter.h"
#include "StringHelper.h"

using namespace coral::cli;
using namespace coral::log;
using namespace coral::helpers;

//-----------------------------------------------------------------------------
class QuitCommand : public InteractiveCommand {
public:

   QuitCommand( bool& quit_signalled )
      : coral::cli::InteractiveCommand( "quit", "Exit the terminal", "exit" )
      , quit_signalled_( quit_signalled )
   {
   }

   ~QuitCommand() {};

   void process( const coral::cli::ArgumentList& args )
   {
      quit_signalled_ = true;
   }

private:

   bool& quit_signalled_;
};

//-----------------------------------------------------------------------------
class ListCommand : public InteractiveCommand {
public:

   ListCommand( const std::map<std::string, std::shared_ptr<InteractiveCommand>>& command_map )
   : coral::cli::InteractiveCommand( "list", "List supported commands" )
   , command_map_( command_map )
   {
   }

   ~ListCommand() {};

   void process(const coral::cli::ArgumentList& args)
   {
      std::map<std::string, std::shared_ptr<InteractiveCommand>>::const_iterator
      command_iterator = command_map_.begin();

      raw("Command List:\n");

      for (; command_iterator != command_map_.end(); ++command_iterator )
      {
         if ( command_iterator->first != command_iterator->second->alias() )
         {
            char command_name_buffer[21];

            if ( command_iterator->second->has_alias() )
            {
               snprintf(
                  command_name_buffer, sizeof(command_name_buffer), "%s[%s]",
                  command_iterator->first.c_str(),
                  command_iterator->second->alias().c_str()
               );
            }
            else
            {
               snprintf(
                  command_name_buffer, sizeof(command_name_buffer), "%s",
                  command_iterator->first.c_str()
               );
            }

            raw( "  %-20s   %s\n",
               command_name_buffer,
               command_iterator->second->brief().c_str()
            );
         }
      }
   }

private:

   const std::map<std::string, std::shared_ptr<InteractiveCommand>>& command_map_;
};

//-----------------------------------------------------------------------------
InteractiveCommandRouter::InteractiveCommandRouter( const std::string& prompt )
   : prompt_          ( prompt )
   , shutdown_message_( "bye" )
   , quit_signalled_  ( false )
{
   add( std::make_shared<ListCommand>( commands_ ) );
   add( std::make_shared<QuitCommand>( quit_signalled_ ) );
}

//-----------------------------------------------------------------------------
InteractiveCommandRouter::~InteractiveCommandRouter()
{
   remove("list");
   remove("quit");
}

//-----------------------------------------------------------------------------
bool InteractiveCommandRouter::add( std::shared_ptr<InteractiveCommand> command_ptr )
{
   bool add_success = false;

   if ( command_ptr )
   {
      add_success = ( commands_.count( command_ptr->command() ) == 0 );

      if ( command_ptr->has_alias() )
      {
         add_success &= ( commands_.count( command_ptr->alias() ) == 0) ;
      }

      if ( add_success )
      {
         commands_.insert( std::make_pair( command_ptr->command(), command_ptr ) );
         commands_.insert( std::make_pair( command_ptr->alias(), command_ptr ) );
      }
   }

   return add_success;
}

//-----------------------------------------------------------------------------
std::shared_ptr<InteractiveCommand> InteractiveCommandRouter::remove( const std::string& command )
{
   std::shared_ptr<InteractiveCommand> command_ptr;

   if ( commands_.count( command ) > 0 )
   {
      command_ptr = commands_.find( command )->second;

      // Remove the main command.
      commands_.erase( command_ptr->command() );

      // If the command has an alias, remove the alias.
      if ( command_ptr->has_alias() )
      {
         commands_.erase( command_ptr->alias() );
      }
   }

   return command_ptr;
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::set_startup_message( const std::string& message )
{
   startup_message_ = message;
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::set_shutdown_message( const std::string& message )
{
   shutdown_message_ = message;
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::run()
{
   std::string command;

   if ( startup_message_.empty() == false )
   {
      raw( "\n%s\n", startup_message_.c_str() );
   }

   raw( "\nType \"list\" to see a list of available commands.\n\n" );

   while ( quit_signalled_ == false )
   {
      raw( "%s ", prompt_.c_str() );

      coral::log::flush();

      process_line( std::cin );
   }

   if ( shutdown_message_.empty() == false )
   {
      raw( "\n%s\n", shutdown_message_.c_str() );
   }

   coral::log::flush();
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::process_line( std::istream& stream )
{
   std::string command;

   std::getline( stream, command );
   if ( ( stream.fail() == false ) && ( command.size() > 0 ) )
   {
      // Split the command string on spaces.
      std::vector<std::string> command_arguments =
         string_helper::split( command, ' ' );

      // The command name is always the first token.
      // The remaining tokens are arguments to the command.
      std::string command_name = command_arguments.front();
      command_arguments.erase(command_arguments.begin());

      CommandTable::iterator command_iterator =
         commands_.find( command_name );
      if ( command_iterator != commands_.end() )
      {
         std::shared_ptr<InteractiveCommand> command_ptr = command_iterator->second;

         if ( command_ptr )
         {
            command_ptr->process( command_arguments );
         }
      }
      else
      {
         error( "Unknown command \"%s\"\n", command_name.c_str() );
      }
   }
}
