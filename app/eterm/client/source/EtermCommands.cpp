#include <string.h>
#include <iostream>
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
    rpc_eterm::LedState lLedState;
    rpc_eterm::Status lStatus;

    // Parse the LED ID
    std::string ledName = args[0];
    if (strcasecmp(ledName.c_str(), "main") == 0)
    {
      lLedState.set_led_id(eterm::MainLed);
    }
    else if (strcasecmp(ledName.c_str(), "fingerprint") == 0)
    {
      lLedState.set_led_id(eterm::FingerprintLed);
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
      if (!lStatus.is_connected())
      {
        std::cout << "Heimdall is not connected." << std::endl;
      }
      else
      {
        if (!lStatus.success())
          std::cout << "Operation failed." << std::endl;
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
FpStatusCommand::FpStatusCommand(HeimdallControllerClientStub& rHeimdall)
: InteractiveCommand("fpstatus", "Get fingerprint reader status")
, mrHeimdall(rHeimdall)
{
}

//-----------------------------------------------------------------------------
void FpStatusCommand::
process(const liber::cli::ArgumentList& args)
{
  rpc_eterm::EmptyParams       emptyParams;
  rpc_eterm::Status            heimdallStatus;

  try {
    mrHeimdall.getFingerprintStatus(emptyParams, heimdallStatus);
    if (!heimdallStatus.success())
    {
      std::cout << "Operation failed. ";
      if (!heimdallStatus.is_connected())
        std::cout << "Heimdall is not connected.";
      std::cout << std::endl;
    }
  } catch (liber::rpc::RpcException& e) {
    std::cout << "Error" << std::endl;
  }
}

