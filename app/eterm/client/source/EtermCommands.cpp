#include <string.h>
#include <iostream>
#include <sstream>
#include "EtermCommands.h"
#include "RpcAppCommon.h"

using namespace liber::cli;
using namespace eterm;

//-----------------------------------------------------------------------------
LedCommand::LedCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("led", "Turn LED on and off")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void LedCommand::
process(const liber::cli::ArgumentList& args)
{
  if (args.size() == 2)
  {
    rpc_eterm::HcSetLedState lLedState;
    rpc_eterm::Status lStatus;

    // Parse the LED ID
    std::string ledName = args[0];
    if (strcasecmp(ledName.c_str(), "main") == 0)
    {
      lLedState.set_led_id(eterm::HC_MAIN_LED);
    }
    else if (strcasecmp(ledName.c_str(), "fingerprint") == 0)
    {
      lLedState.set_led_id(eterm::HC_FP_LED);
    }
    else
    {
      std::cout << "Invalid LED ID. Specify \"main\" or \"fingerprint\"."
                << std::endl;
      return;
    }

    // Parse the LED state.
    std::string ledState = args[1];
    if (strcasecmp(ledState.c_str(), "on") == 0)
    {
      lLedState.set_led_on(true);
    }
    else if (strcasecmp(ledState.c_str(), "off") == 0)
    {
      lLedState.set_led_on(false);
    }
    else
    {
      std::cout << "Invalid option. Specify \"on\" or \"off\"." << std::endl;
      return;
    }

    try {
      mrHeimdall.setLedState(lLedState, lStatus);
      if (!lStatus.connected())
      {
        std::cout << "Operation failed. Heimdall is not connected."
                  << std::endl;
      }
      else if (lStatus.busy())
      {
        std::cout << "Heimdall is busy. Try back later." << std::endl;
      }
    } catch (liber::rpc::RpcException& e) {
      std::cout << "Error" << std::endl;
    }
  }
  else
  {
    std::cout << "Too few arguments.  Specify \"led main/fingerprint on/off\"."
              << std::endl;
  }
}


//-----------------------------------------------------------------------------
ActivateDoorCommand::ActivateDoorCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("activatedoor", "Open or close the door")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void ActivateDoorCommand::
process(const liber::cli::ArgumentList& args)
{
  rpc_eterm::EmptyParams       emptyParams;
  rpc_eterm::Status            heimdallStatus;

  try {
    mrHeimdall.activateDoor(emptyParams, heimdallStatus);
    if (!heimdallStatus.connected())
    {
      std::cout << "Operation failed. Heimdall is not connected."
                << std::endl;
    }
    else if (heimdallStatus.busy())
    {
      std::cout << "Heimdall is busy. Try back later." << std::endl;
    }
  } catch (liber::rpc::RpcException& e) {
    std::cout << "Error" << std::endl;
  }
}


//-----------------------------------------------------------------------------
EnrollUserCommand::EnrollUserCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("enroll", "Enroll a new user")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void EnrollUserCommand::
process(const liber::cli::ArgumentList& args)
{
  rpc_eterm::UserName userName;
  rpc_eterm::Status   heimdallStatus;

  // The user must supply the user's first and last name.
  if (args.size() == 2)
  {
    userName.set_first_name(args[0]);
    userName.set_last_name(args[1]);
    try {
      mrHeimdall.enroll(userName, heimdallStatus);
      if (!heimdallStatus.connected())
      {
        std::cout << "Operation failed. Heimdall is not connected."
                  << std::endl;
      }
      else if (heimdallStatus.busy())
      {
        std::cout << "Heimdall is busy. Try back later." << std::endl;
      }
      else
      {
        std::cout << "Enrollment started. Proceed to register fingerprint." << std::endl;
      }
    } catch (liber::rpc::RpcException& e) {
      std::cout << "Error" << std::endl;
    }
  }
  else
  {
    std::cout << "Please specify the user's first and last name." << std::endl;
  }
}


//-----------------------------------------------------------------------------
RemoveOneCommand::RemoveOneCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("removeone", "Remove a user")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void RemoveOneCommand::
process(const liber::cli::ArgumentList& args)
{
  rpc_eterm::Status heimdallStatus;

  // The user must supply the user's first and last name.
  if (args.size() == 1)
  {
    std::stringstream ss;
    ss << args[1];

    int lUserId = 0;
    ss >> lUserId;
    if (ss.fail())
    {
      std::cout << "Parsing failed. Please specify user ID as an integer."
                << std::endl;
    }
    else if (lUserId < 1 || lUserId > 199)
    {
      std::cout << "Invalid user ID.  ID must be > 0 and < 200." << std::endl;
    }
    else
    {
      rpc_eterm::UserID userID;
      userID.set_user_id(lUserId);

      try {
        mrHeimdall.removeOne(userID, heimdallStatus);
        if (!heimdallStatus.connected())
        {
          std::cout << "Operation failed. Heimdall is not connected."
                    << std::endl;
        }
        else if (heimdallStatus.busy())
        {
          std::cout << "Heimdall is busy. Try back later." << std::endl;
        }
        else
        {
          std::cout << "Enrollment started. Proceed to register fingerprint." << std::endl;
        }
      } catch (liber::rpc::RpcException& e) {
        std::cout << "Error" << std::endl;
      }
    }
  }
  else
  {
    std::cout << "Please specify an individual user's ID."
              << std::endl;
  }
}


//-----------------------------------------------------------------------------
RemoveAllCommand::RemoveAllCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("removeall", "Remove all registered users")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void RemoveAllCommand::
process(const liber::cli::ArgumentList& args)
{
  rpc_eterm::EmptyParams       emptyParams;
  rpc_eterm::Status            heimdallStatus;

  try {
    mrHeimdall.removeAll(emptyParams, heimdallStatus);
    if (!heimdallStatus.connected())
    {
      std::cout << "Operation failed. Heimdall is not connected."
                << std::endl;
    }
    else if (heimdallStatus.busy())
    {
      std::cout << "Heimdall is busy. Try back later." << std::endl;
    }
  } catch (liber::rpc::RpcException& e) {
    std::cout << "Error" << std::endl;
  }
}


//-----------------------------------------------------------------------------
PrintUsersCommand::PrintUsersCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("printusers", "Print all registered users")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void printUser(const rpc_eterm::User& user)
{
  std::cout << "  User: " << user.user_name().last_name() << ", "
                          << user.user_name().first_name() << std::endl
            << "        ID: " << user.user_id() << std::endl
            << "        access count: " << user.access_count() << std::endl
            << "        last access: " << user.last_access().seconds() << "."
                                       << user.last_access().nanoseconds()
                                       << std::endl << std::endl;
}

//-----------------------------------------------------------------------------
void PrintUsersCommand::
process(const liber::cli::ArgumentList& args)
{
  rpc_eterm::EmptyParams emptyParams;
  rpc_eterm::UserList    userList;

  try {
    mrHeimdall.getUsers(emptyParams, userList);
    std::cout << "Registered Heimdall users:" << std::endl;
    if (userList.users_size() <= 0)
    {
      std::cout << "  No registered users." << std::endl;
    }
    else
    {
      for (int index = 0; index < userList.users_size(); index++)
      {
        printUser(userList.users(index));
      }
    }
  } catch (liber::rpc::RpcException& e) {
    std::cout << "Error" << std::endl;
  }
}


