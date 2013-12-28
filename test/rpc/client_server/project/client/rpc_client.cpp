#include <iostream>
#include "ArgParser.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "TcpClient.h"
#include "CalculatorClientStub.h"
#include "DatabaseClientStub.h"
#include "ClientPacketRouter.h"
#include "RpcAppCommon.h"

using namespace liber::rpc;
using namespace liber::netapp;
using namespace liber::net;
using namespace rpc_getting_started;


void calculator_run(liber::rpc::RpcClient &client)
{
   printf("calculator_run\n");

   CalculatorClientStub calculator(client);
   CalculatorClientStub calcb(client);
   
   Parameters params;
   Result     result;

   try {
      params.add_values(5);
      params.add_values(4);
      calcb.add(params, result);
      calcb.div(params, result);
   } catch (RpcException& e) {
      std::cout << "Error" << std::endl;
   }
   
   try {
      params.clear_values();
      result.Clear();
      params.add_values(5);
      params.add_values(4);
      calculator.add(params, result);
      std::cout << "5 + 4 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Add failed!" << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      params.add_values(5);
      params.add_values(4);
      calculator.sub(params, result);
      std::cout << "5 - 4 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Sub failed!" << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      params.add_values(5);
      params.add_values(4);
      calculator.mul(params, result);
      std::cout << "5 * 4 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Mul failed!" << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      params.add_values(10);
      params.add_values(5);
      calculator.div(params, result);
      std::cout << "10 / 5 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Div failed:" << std::endl
                << e.trace()
                << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      calculator.getNumOps(params, result);
      std::cout << "# of ops = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "getNumOps failed" << std::endl;
   }
   
   try {
      params.clear_values();
      result.Clear();
      calcb.getNumOps(params, result);
      std::cout << "calcb # of ops = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "calcb.getNumOps() failed" << std::endl;
   }
}

void database_run(liber::rpc::RpcClient& client)
{
   printf("database_run\n");
   DatabaseClientStub database(client);

   DbStatus status;
   UserAttributes attributes;
   UserName name;

   try {
      name.set_first("Ted");
      name.set_last("Nugent");
      attributes.set_allocated_name(new UserName(name));
      attributes.set_age(65);
      attributes.set_eye_color("brown");
      database.create(attributes, status);

      if (!status.success())
      {
         std::cout << "Failed to create Ted Nugent." << std::endl;
      }
   } catch (RpcException& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      status.Clear();
      name.set_first("Gloria");
      name.set_last("Elmsworth");
      attributes.set_allocated_name(new UserName(name));
      attributes.set_age(83);
      attributes.set_eye_color("grey");
      database.create(attributes, status);

      if (!status.success())
      {
         std::cout << "Failed to create Gloria Elmsworth." << std::endl;
      }
   } catch (RpcException& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      status.Clear();
      name.set_first("Paul");
      name.set_last("Newman");
      attributes.set_allocated_name(new UserName(name));
      attributes.set_age(50);
      attributes.set_eye_color("blue");
      database.create(attributes, status);

      if (!status.success())
      {
         std::cout << "Failed to create Paul Newman." << std::endl;
      }
   } catch (RpcException& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      UserAttributesList attrList;
      name.Clear();
      attributes.Clear();

      name.set_first("Paul");
      name.set_last("Newman");
      database.read(name, attrList);
      if (attrList.users_size() == 0)
      {
         std::cout << "Failed to read Paul Newman." << std::endl;
      }
      else {
         for (int ind = 0; ind < attrList.users_size(); ind++)
         {
            std::cout << "Paul's info:" << std::endl;
            std::cout << "  lastname:" << attrList.users(ind).name().last() << std::endl;
            std::cout << "  age:" << attrList.users(ind).age() << std::endl;
            std::cout << "  eye color:" << attrList.users(ind).eye_color() << std::endl;
         }
      }
   } catch (RpcException& e) {
      std::cout << "database.read failed: " << e.toString() << std::endl;
   }
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
 
  ClientPacketRouter router(*lpSocket);
  RpcClient client;

  // Register the RPC client application.
  if (!router.addSubscriber(RPC_APP_ID, &client))
  {
    std::cout << "Failed to RPC as packet subscriber." << std::endl;
  }

  // Start the client packet router.
  router.start();

  calculator_run(client);
  database_run(client);

  // Stop the client router.
  router.stop();

  if (lpSocket)
  {
    delete lpSocket;
    lpSocket = NULL;
  }
   
  return 0;
}


