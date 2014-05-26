#include <map>
#include <vector>
#include <utility>
#include <sstream>
#include <google/protobuf/io/printer.h>
#include "StringHelper.h"
#include "LiberFileGenerator.h"

#define PRINTER_DELIMETER        ('$')

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::compiler;

//-----------------------------------------------------------------------------
LiberFileGenerator::
LiberFileGenerator(
  std::ofstream& debug,
  const FileDescriptor* pDescriptor,
  GeneratorContext* pContext)
: mDebug(debug)
, mpDescriptor(pDescriptor)
, mpContext(pContext)
{
}

//-----------------------------------------------------------------------------
LiberFileGenerator::~LiberFileGenerator()
{
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateServices()
{
  bool lbSuccess = true;

  mVars["method_separator"] = "//--------------------------------------"
                              "---------------------------------------\n";

  int lnServiceCount = mpDescriptor->service_count();
  for (int lnService = 0; lnService < lnServiceCount; lnService++)
  {
    const ServiceDescriptor* lpService = NULL;

    lpService = mpDescriptor->service(lnService);

    mVars.insert(std::make_pair("service", lpService->name()));

    generateClientResourceStubHeader(lpService);
    generateClientResourceStubImp(lpService);

    generateServerResourceStubHeader(lpService);
    generateServerResourceStubImp(lpService);

    generateServerInstanceWrapperHeader(lpService);
    generateServerInstanceWrapperImp(lpService);
    generateServerInstanceWrapperUserImp(lpService);
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateClientResourceStubHeader(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = pService->name() + "ClientStub.h";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    // Add common variables to the map.
    //mVars.insert(std::make_pair<std::string,std::string>("service", pService->name()));

    // Compiler guard.
    printer.Print("#ifndef $service$_CLIENT_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name())); 
    printer.Print("#define $service$_CLIENT_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name())); 
    printer.Print("\n");

    printer.Print("#include \"RpcClientResource.h\"\n");
    printer.Print("#include \"$package$.pb.h\"\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    // Add the service to the same namespace.
    printer.Print("namespace $package$ {\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    printer.Print(mVars, "class $service$ClientStub\n"
                  ": public liber::rpc::RpcClientResource {\n");
    printer.Print("public:\n");
    printer.Print("\n");

    // Constructor and desctructor.
    printer.Indent();
    printer.Print(mVars, "explicit $service$ClientStub(liber::rpc::RpcClient &client);\n");
    printer.Print(mVars, "~$service$ClientStub();\n");
    printer.Print("\n");

    int lnMethodCount = pService->method_count();
    for (int lnMethod = 0; lnMethod < lnMethodCount; lnMethod++)
    {
      const MethodDescriptor* lpMethod = pService->method(lnMethod);
      printer.Print("void $method$(const $request$& request, $response$& response, liber::rpc::AsyncRpcSupervisor* pSupervisor = NULL)"
                    "\n       throw (liber::rpc::RpcException);\n",
                    "method", lpMethod->name(),
                    "request", GetFullyQualifiedName(lpMethod->input_type()),
                    "response", GetFullyQualifiedName(lpMethod->output_type()));
    }
    printer.Print("\n");

    // Close the resource definition.
    printer.Outdent();
    printer.Print(mVars, "}; // End $service$ClientStub\n");
    printer.Print("\n");

    // Close the namespace.
    printer.Print("}  // End namespace $package$\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    // Close the compiler guard.
    printer.Print("\n#endif // $service$_CLIENT_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name())); 

    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateClientResourceStubImp(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = pService->name() + "ClientStub.cpp";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    // Includes
    printer.Print("#include \"$service$ClientStub.h\"\n", "service",
                  pService->name());
    printer.Print("\n");

    // Namespaces
    printer.Print("using namespace liber::rpc;\n");
    printer.Print("using namespace $package$;\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    // Constructor
    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "$service$ClientStub::\n"
                  "$service$ClientStub(RpcClient &client)\n");
    printer.Print(mVars, ": RpcClientResource(client, \"$service$\")\n");
    printer.Print("{\n");
    printer.Print("}\n");
    printer.Print("\n");

    // Destructor
    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "$service$ClientStub::\n"
                  "~$service$ClientStub()\n");
    printer.Print("{\n");
    printer.Print("}\n");
    printer.Print("\n");

    // Methods.
    int lnMethodCount = pService->method_count();
    for (int lnMethod = 0; lnMethod < lnMethodCount; lnMethod++)
    {
      const MethodDescriptor* lpMethod = pService->method(lnMethod);
      printer.Print(mVars, "$method_separator$");
      printer.Print(mVars, "void $service$ClientStub::\n");
      printer.Print("$method$(const $request$& request, $response$& response, AsyncRpcSupervisor* pSupervisor)\n"
                    " throw (RpcException)\n",
                    "method", lpMethod->name(),
                    "request", GetFullyQualifiedName(lpMethod->input_type()),
                    "response", GetFullyQualifiedName(lpMethod->output_type()));
      printer.Print("{\n");
      printer.Indent();
      printer.Print("if (!call(\"$method$\", request, response, pSupervisor))\n",
                    "method", lpMethod->name());
      printer.Print("{\n");
      printer.Indent();
      printer.Print("throw getLastError();\n");
      printer.Outdent();
      printer.Print("}\n");
      printer.Outdent();
      printer.Print("}\n");
      printer.Print("\n");
    }
    printer.Print("\n");

    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateServerResourceStubHeader(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = pService->name() + "ServerStub.h";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    // Compiler guard.
    printer.Print("#ifndef $service$_SERVER_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name()));
    printer.Print("#define $service$_SERVER_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name()));
    printer.Print("\n");

    // Includes
    printer.Print("#include \"RpcServerResource.h\"\n");
    printer.Print(mVars, "#include \"$service$Wrapper.h\"\n");
    printer.Print("\n");

    // Add the service to the same namespace.
    printer.Print("namespace $package$ {\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    printer.Print(mVars, "struct $service$HookCb;\n");
    printer.Print("\n");

    printer.Print(mVars, "class $service$ServerStub\n"
                  ": public liber::rpc::RpcServerResource {\n");
    printer.Print("public:\n");
    printer.Print("\n");

    // Constructor and desctructor.
    printer.Indent();
    printer.Print(mVars, "explicit $service$ServerStub();\n");
    printer.Print(mVars, "~$service$ServerStub();\n");
    printer.Print("\n");

    //
    printer.Print("virtual void registerActions();\n");
    printer.Print("\n");

    printer.Print(mVars, "typedef void (*$service$Hook)($service$Wrapper*, void*);\n");
    printer.Print(mVars, "bool registerCtorHook($service$Hook pFunc, void* pUserData = NULL);\n");
    printer.Print(mVars, "bool registerDtorHook($service$Hook pFunc, void* pUserData = NULL);\n");
    printer.Print("\n");

    printer.Outdent();
    printer.Print("protected:\n");
    printer.Indent();
    printer.Print("\n");

    printer.Print("liber::rpc::InstanceWrapper* createInstance();\n");
    printer.Print("\n");

    printer.Print("bool tugCtorHook(liber::rpc::InstanceWrapper* pInst);\n");
    printer.Print("bool tugDtorHook(liber::rpc::InstanceWrapper* pInst);\n");
    printer.Print("\n");

    printer.Outdent();
    printer.Print("private:\n");
    printer.Indent();
    printer.Print("\n");

    printer.Print(mVars, "struct $service$HookCb* mpCreateHook;\n");
    printer.Print(mVars, "struct $service$HookCb* mpDestroyHook;\n");
    printer.Print("\n");

    // Close the resource definition.
    printer.Outdent();
    printer.Print(mVars, "}; // End $service$ServerStub\n");
    printer.Print("\n");

    // Close the namespace.
    printer.Print("}  // End namespace $package$\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    // Close the compiler guard.
    printer.Print("\n#endif // $service$_SERVER_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name()));

    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateServerResourceStubImp(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = pService->name() + "ServerStub.cpp";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    printer.Print(mVars, "#include \"$service$ServerStub.h\"\n");
    printer.Print("\n");
    printer.Print("using namespace liber::rpc;\n");
    printer.Print("using namespace $package$;\n",
                  "package", pService->file()->package());
    printer.Print("\n");

    printer.Print("namespace $package$ {\n", "package", pService->file()->package());
    printer.Print(mVars, "struct $service$HookCb {\n");
    printer.Indent();
    printer.Print(mVars, "$service$ServerStub::$service$Hook pFunc;\n");
    printer.Print("void* pUserData;\n");
    printer.Outdent();
    printer.Print("};\n");
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "$service$ServerStub::$service$ServerStub()\n");
    printer.Print(mVars, ": RpcServerResource(\"$service$\")\n");
    printer.Print(", mpCreateHook(NULL)\n");
    printer.Print(", mpDestroyHook(NULL)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print(mVars, "mpCreateHook = new $service$HookCb;\n");
    printer.Print("mpCreateHook->pFunc = NULL;\n");
    printer.Print("mpCreateHook->pUserData = NULL;\n");
    printer.Print("\n");
    printer.Print(mVars, "mpDestroyHook = new $service$HookCb;\n");
    printer.Print("mpDestroyHook->pFunc = NULL;\n");
    printer.Print("mpDestroyHook->pUserData = NULL;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "$service$ServerStub::\n~$service$ServerStub()\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("if (mpCreateHook)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("delete mpCreateHook;\n");
    printer.Print("mpCreateHook = NULL;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print("if (mpDestroyHook)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("delete mpDestroyHook;\n");
    printer.Print("mpDestroyHook = NULL;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "void $service$ServerStub::registerActions()\n");
    printer.Print("{\n");
    printer.Indent();
    for (int lnMethod = 0; lnMethod < pService->method_count(); lnMethod++)
    {
      const MethodDescriptor* lpMethod = pService->method(lnMethod);
      printer.Print("addAction(\"$method$\", "
                    "$service$Wrapper::$method$Delegate);\n",
                    "method", lpMethod->name(),
                    "service", pService->name());
    }
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "bool $service$ServerStub::\n"
                  "registerCtorHook($service$Hook pFunc, void* pUserData)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);\n");
    printer.Print("\n");
    printer.Print("if (lbSuccess)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("if (mpCreateHook)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("mpCreateHook->pFunc = pFunc;\n");
    printer.Print("mpCreateHook->pUserData = pUserData;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("mHookMutex.unlock();\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("return lbSuccess;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "bool $service$ServerStub::\n"
                  "registerDtorHook($service$Hook pFunc, void* pUserData)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);\n");
    printer.Print("\n");
    printer.Print("if (lbSuccess)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("if (mpDestroyHook)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("mpDestroyHook->pFunc     = pFunc;\n");
    printer.Print("mpDestroyHook->pUserData = pUserData;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("mHookMutex.unlock();\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("return lbSuccess;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "InstanceWrapper* $service$ServerStub::createInstance()\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print(mVars, "return new $service$Wrapper();\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "bool $service$ServerStub::tugCtorHook(InstanceWrapper* pInst)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);\n");
    printer.Print("\n");
    printer.Print("if (lbSuccess)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("if (pInst && mpCreateHook && mpCreateHook->pFunc)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print(mVars, "$service$Wrapper* lpInst = dynamic_cast<$service$Wrapper*>(pInst);\n");
    printer.Print("if (lpInst)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("mpCreateHook->pFunc(lpInst, mpCreateHook->pUserData);\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("mHookMutex.unlock();\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("return lbSuccess;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "bool $service$ServerStub::tugDtorHook(InstanceWrapper* pInst)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);\n");
    printer.Print("\n");
    printer.Print("if (lbSuccess)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("if (pInst && mpDestroyHook && mpDestroyHook->pFunc)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print(mVars, "$service$Wrapper* lpInst = dynamic_cast<$service$Wrapper*>(pInst);\n");
    printer.Print("if (lpInst)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("mpDestroyHook->pFunc(lpInst, mpDestroyHook->pUserData);\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("mHookMutex.unlock();\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print("return lbSuccess;\n");
    printer.Outdent();
    printer.Print("}\n");

    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateServerInstanceWrapperHeader(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = pService->name() + "Wrapper.h";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    // Compiler guard.
    printer.Print("#ifndef $service$_WRAPPER_H\n", "service",
                  StringHelper::ToUpper(pService->name()));
    printer.Print("#define $service$_WRAPPER_H\n", "service",
                  StringHelper::ToUpper(pService->name()));
    printer.Print("\n");

    // Includes
    printer.Print("#include \"InstanceWrapper.h\"\n");
    printer.Print("#include \"$package$.pb.h\"\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    // Add the service to the same namespace.
    printer.Print("namespace $package$ {\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    printer.Print(mVars, "class $service$Wrapper\n");
    printer.Print(": public liber::rpc::InstanceWrapper {\n");
    printer.Print("public:\n");
    printer.Print("\n");

    // Constructor and desctructor.
    printer.Indent();
    printer.Print(mVars, "explicit $service$Wrapper();\n");
    printer.Print(mVars, "~$service$Wrapper();\n");
    printer.Print("\n");

    printer.Print("bool initialize(const std::string& params);\n");
    printer.Print("bool destroy(const std::string& params);\n");
    printer.Print("\n");

    for (int lnMethod = 0; lnMethod < pService->method_count(); lnMethod++)
    {
      printer.Print("static void DELEGATE($method$Delegate);\n",
                    "method", pService->method(lnMethod)->name());
    }
    printer.Print("\n");

    printer.Outdent();
    printer.Print("private:\n");
    printer.Print("\n");

    printer.Indent();
    for (int lnMethod = 0; lnMethod < pService->method_count(); lnMethod++)
    {
      printer.Print("void ACTION($method$, $request_type$, $response_type$);\n",
                    "method", pService->method(lnMethod)->name(),
                    "request_type", GetFullyQualifiedName(pService->method(lnMethod)->input_type()),
                    "response_type", GetFullyQualifiedName(pService->method(lnMethod)->output_type()));
    }
    printer.Print("\n");

    printer.Outdent();
    printer.Print("private:\n");
    printer.Print("\n");

    printer.Indent();
    printer.Print("// TODO: Add additional instance variables here.\n");
    printer.Print("\n");

    // Close the resource definition.
    printer.Outdent();
    printer.Print(mVars, "}; // End $service$Wrapper\n");
    printer.Print("\n");

    // Close the namespace.
    printer.Print("}  // End namespace $package$\n", "package",
                  pService->file()->package());
    printer.Print("\n");

    // Close the compiler guard.
    printer.Print("\n#endif // $service$_CLIENT_STUB_H\n", "service",
                  StringHelper::ToUpper(pService->name()));



    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateServerInstanceWrapperImp(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = pService->name() + "Wrapper.cpp";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    printer.Print("#include \"$package$.pb.h\"\n", "package", pService->file()->package());
    printer.Print(mVars, "#include \"$service$Wrapper.h\"\n");
    printer.Print("\n");
    printer.Print("using namespace liber::rpc;\n");
    printer.Print("using namespace $package$;\n", "package", pService->file()->package());
    printer.Print("\n");
    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "$service$Wrapper::$service$Wrapper()\n");
    printer.Print(": InstanceWrapper()\n");
    printer.Print("{\n");
    printer.Print("}\n");
    printer.Print("\n");
    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "$service$Wrapper::~$service$Wrapper()\n");
    printer.Print("{\n");
    printer.Print("}\n");
    printer.Print("\n");

    for (int lnMethod = 0; lnMethod < pService->method_count(); lnMethod++)
    {
      const MethodDescriptor* lpMethod = pService->method(lnMethod);

      printer.Print(mVars, "$method_separator$");
      printer.Print("void $service$Wrapper::DELEGATE($method$Delegate)\n",
                    "service", pService->name(),
                    "method",  lpMethod->name());
      printer.Print("{\n");
      printer.Indent();
      printer.Print(mVars, "$service$Wrapper* lpInst = dynamic_cast<$service$Wrapper*>(pInst);\n");
      printer.Print("$request_type$ requestMessage;\n", "request_type", GetFullyQualifiedName(lpMethod->input_type()));
      printer.Print("$response_type$ responseMessage;\n", "response_type", GetFullyQualifiedName(lpMethod->output_type()));
      printer.Print("\n");

      printer.Print("e.pushFrame(TraceFrame(\"$service$Wrapper\", \"$method$\","
                    "\n           __FILE__, __LINE__));\n",
                    "service", pService->name(),
                    "method",  lpMethod->name());
      printer.Print("\n");

      printer.Print("requestMessage.ParseFromString(request);\n");

      printer.Print("if (!requestMessage.IsInitialized())\n");
      printer.Print("{\n");
      printer.Indent();
      printer.Print("e.id = MissingParameters;\n");
      printer.Print("e.message = \"Parameters not initialized\";\n");
      printer.Print("return;\n");
      printer.Outdent();
      printer.Print("}\n");
      printer.Print("\n");
      printer.Print("lpInst->$method$(requestMessage, responseMessage, e);\n",
                    "method", lpMethod->name());
      printer.Print("responseMessage.SerializeToString(&response);\n");
      printer.Print("\n");
      printer.Print("e.popFrame();\n");
      printer.Outdent();
      printer.Print("}\n");
      printer.Print("\n");
    }

    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::
generateServerInstanceWrapperUserImp(const ServiceDescriptor* pService)
{
  bool lbFailed = true;
  std::string filename = "user." + pService->name() + "Wrapper.cpp";

  // Open a stream to the output file.
  ZeroCopyOutputStream* pStream = mpContext->Open(filename);

  if (pStream)
  {
    Printer printer(pStream, PRINTER_DELIMETER);
    mDebug << "LiberFileGenerator: Generating " << filename << "." << std::endl;

    printer.Print(mVars, "#include \"$service$Wrapper.h\"\n");
    printer.Print("\n");
    printer.Print("using namespace liber::rpc;\n");
    printer.Print("using namespace $package$;\n",
                  "package", pService->file()->package());
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "bool $service$Wrapper::initialize(const std::string& params)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("bool lbSuccess = true;\n");
    printer.Print("\n");
    printer.Print("// TODO: Perform instance initialization.\n");
    printer.Print("\n");
    printer.Print("return lbSuccess;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    printer.Print(mVars, "$method_separator$");
    printer.Print(mVars, "bool $service$Wrapper::destroy(const std::string& params)\n");
    printer.Print("{\n");
    printer.Indent();
    printer.Print("bool lbSuccess = true;\n");
    printer.Print("\n");
    printer.Print("// TODO: Destroy instance.\n");
    printer.Print("\n");
    printer.Print("return lbSuccess;\n");
    printer.Outdent();
    printer.Print("}\n");
    printer.Print("\n");

    for (int lnMethod = 0; lnMethod < pService->method_count(); lnMethod++)
    {
      const MethodDescriptor* lpMethod = pService->method(lnMethod);

      printer.Print(mVars, "$method_separator$");
      printer.Print(mVars, "void $service$Wrapper::\n");
      printer.Print("$method$(const $request_type$& request, $response_type$& response, RpcException& e)\n",
                    "method", lpMethod->name(),
                    "request_type", GetFullyQualifiedName(lpMethod->input_type()),
                    "response_type", GetFullyQualifiedName(lpMethod->output_type()));
      printer.Print("{\n");
      printer.Indent();
      printer.Print("// TODO: Implement action here and remove the following two lines.\n");
      printer.Print("e.id = UnimplementedAction;\n");
      printer.Print("e.message = \"$service$Wrapper::$method$ has not been implemented.\";\n",
                    "service", pService->name(),
                    "method", lpMethod->name());
      printer.Outdent();
      printer.Print("}\n");
      printer.Print("\n");
    }

    lbFailed = printer.failed();
    delete pStream;
  }

  return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
std::string LiberFileGenerator::GetFullyQualifiedName(const Descriptor* pDescriptor)
{
  std::stringstream lFullQualName;
  std::vector<std::string> lTypeStack;

  const Descriptor* lpTempDescriptor = pDescriptor;
  while (lpTempDescriptor != NULL)
  {
    lTypeStack.push_back(lpTempDescriptor->name());
    lpTempDescriptor = lpTempDescriptor->containing_type();
  }

  while (!lTypeStack.empty())
  {
    lFullQualName << lTypeStack.back();
    lTypeStack.pop_back();

    if (!lTypeStack.empty())
    {
      lFullQualName << "_";
    }
  }

  return lFullQualName.str();
} 

