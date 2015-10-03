#include <utility>
#include <iostream>
#include <iomanip>
#include "Log.h"
#include "InteractiveCommandRouter.h"
#include "StringHelper.h"

using namespace liber::cli;
using namespace liber::log;

//-----------------------------------------------------------------------------
class QuitCommand : public InteractiveCommand {
public:

   QuitCommand( bool& quit_signalled )
      : liber::cli::InteractiveCommand( "quit", "Exit the terminal", "exit" )
      , quit_signalled_( quit_signalled )
   {
   }

   ~QuitCommand() {};

   void process( const liber::cli::ArgumentList& args )
   {
      quit_signalled_ = true;
   }

private:

   bool& quit_signalled_;
};

//-----------------------------------------------------------------------------
class ListCommand : public InteractiveCommand {
public:

   ListCommand( const std::map<std::string, InteractiveCommand*>& command_map )
   : liber::cli::InteractiveCommand( "list", "List supported commands" )
   , command_map_( command_map )
   {
   }

   ~ListCommand() {};

   void process(const liber::cli::ArgumentList& args)
   {
      std::map<std::string, InteractiveCommand*>::const_iterator
      command_iterator = command_map_.begin();

      raw("Command List:\n");

      for (; command_iterator != command_map_.end(); ++command_iterator )
      {
         if ( command_iterator->first != command_iterator->second->alias() )
         {
            char command_name_buffer[21];

            if ( command_iterator->second->hasAlias() )
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

   const std::map<std::string, InteractiveCommand*>& command_map_;
};

//-----------------------------------------------------------------------------
InteractiveCommandRouter::InteractiveCommandRouter( const std::string& prompt )
   : prompt_          ( prompt )
   , shutdown_message_( "bye" )
   , quit_signalled_  ( false )
{
   add( new ListCommand( commands_ ) );
   add( new QuitCommand( quit_signalled_ ) );
}

//-----------------------------------------------------------------------------
InteractiveCommandRouter::~InteractiveCommandRouter()
{
   InteractiveCommand* command_ptr = NULL;

   command_ptr = remove("list");
   if ( command_ptr != NULL )
   {
      delete command_ptr;
      command_ptr = NULL;
   }

   command_ptr = remove("quit");
   if ( command_ptr != NULL )
   {
      delete command_ptr;
      command_ptr = NULL;
   }
}

//-----------------------------------------------------------------------------
bool InteractiveCommandRouter::add( InteractiveCommand* command_ptr )
{
   bool add_success = false;

   if ( command_ptr )
   {
      add_success = ( commands_.count( command_ptr->command() ) == 0 );

      if ( command_ptr->hasAlias() )
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
InteractiveCommand* InteractiveCommandRouter::remove( const std::string& command )
{
   InteractiveCommand* command_ptr = NULL;

   if ( commands_.count( command ) > 0 )
   {
      command_ptr = commands_.find( command )->second;

      // Remove the main command.
      commands_.erase( command_ptr->command() );

      // If the command has an alias, remove the alias.
      if ( command_ptr->hasAlias() )
      {
         commands_.erase( command_ptr->alias() );
      }
   }

   return command_ptr;
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::setStartupMessage( const std::string& message )
{
   startup_message_ = message;
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::setShutdownMessage( const std::string& message )
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
      liber::log::flush();

      // std::getline(std::cin, command);
      // if ( ( std::cin.fail() == false ) && ( command.size() > 0 ) )
      // {
      //    // Split the command string on spaces.
      //    std::vector<std::string> command_arguments =
      //       StringHelper::Split( command, ' ' );

      //    // The command name is always the first token.
      //    // The remaining tokens are arguments to the command.
      //    std::string command_name = command_arguments.front();
      //    command_arguments.erase(command_arguments.begin());

      //    CommandTable::iterator command_iterator =
      //       commands_.find( command_name );
      //    if ( command_iterator != commands_.end() )
      //    {
      //       InteractiveCommand* command_ptr = command_iterator->second;

      //       if ( command_ptr )
      //       {
      //          command_ptr->process( command_arguments );
      //       }
      //    }
      //    else
      //    {
      //       error( "Unknown command \"%s\"\n", command_name.c_str() );
      //    }
      // }
      processLine( std::cin );
   }

   if ( shutdown_message_.empty() == false )
   {
      raw( "\n%s\n", shutdown_message_.c_str() );
   }

   liber::log::flush();
}

//-----------------------------------------------------------------------------
void InteractiveCommandRouter::processLine( std::istream& stream )
{
   std::string command;

   std::getline( stream, command );
   if ( ( stream.fail() == false ) && ( command.size() > 0 ) )
   {
      // Split the command string on spaces.
      std::vector<std::string> command_arguments =
         StringHelper::Split( command, ' ' );

      // The command name is always the first token.
      // The remaining tokens are arguments to the command.
      std::string command_name = command_arguments.front();
      command_arguments.erase(command_arguments.begin());

      CommandTable::iterator command_iterator =
         commands_.find( command_name );
      if ( command_iterator != commands_.end() )
      {
         InteractiveCommand* command_ptr = command_iterator->second;

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
