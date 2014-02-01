#include <iostream>
#include "ArgParser.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "TcpClient.h"
//#include "HeimdallControllerClientStub.h"
#include "ClientPacketRouter.h"
#include "RpcAppCommon.h"
#include "EtermCommands.h"
#include "InteractiveCommandRouter.h"

using namespace liber::rpc;
using namespace liber::netapp;
using namespace liber::net;
using namespace liber::cli;
//using namespace eterm;

void runClient(RpcClient& client)
{
  HeimdallControllerClientStub heimdallClient(client);
  eterm::LedCommand ledCommand(heimdallClient);
  eterm::FpStatusCommand fpCommand(heimdallClient);

  InteractiveCommandRouter commandRouter;
  commandRouter.add(&ledCommand);
  commandRouter.add(&fpCommand);

  commandRouter.run();
}

int main(int argc, char *argv[])
{
  ArgParser args;
  args.addArg("name: HostName, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");
  args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");

  if (!args.parse((const char**)argv, argc))
  {
    args.printArgErrors(true);
    return 1;
  }

  std::string lsHostName;
  args.getArgVal(lsHostName, Argument::ArgName, "HostName");

  int lnPort = 0;
  args.getArgVal(lnPort, Argument::ArgName, "Port");

  TcpSocket* lpSocket = TcpClient::Connect(lsHostName.c_str(), lnPort, 2000);

  if (lpSocket == NULL)
  {
    std::cout << "Failed to connect to server!" << std::endl;
    return -1;
  }
 
  ClientPacketRouter router(*lpSocket);
  RpcClient client;

  // Register the RPC client application.
  if (!router.addSubscriber(RPC_APP_ID, &client))
  {
    std::cout << "Failed to RPC as packet subscriber." << std::endl;
  }

  // Start the client packet router.
  router.start();
  /*
  HeimdallControllerClientStub heimdallClient(client);
  LedCommand ledCommand(heimdallClient);

  InteractiveCommandRouter commandRouter;
  commandRouter.add(&ledCommand);

  commandRouter.run();
  */
  runClient(client);

  // Stop the client router.
  router.stop();

  if (lpSocket)
  {
    delete lpSocket;
    lpSocket = NULL;
  }
   
  return 0;
}


