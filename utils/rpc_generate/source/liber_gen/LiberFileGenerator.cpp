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
bool LiberFileGenerator::generateServices()
{
   bool lbSuccess = true;

   mVars["method_separator"] = "//--------------------------------------"
                              "---------------------------------------\n";

   int lnServiceCount = mpDescriptor->service_count();
   for (int lnService = 0; lnService < lnServiceCount; lnService++)
   {
      const ServiceDescriptor* service_ptr = NULL;

      service_ptr = mpDescriptor->service(lnService);

      mVars.insert(std::make_pair("service", service_ptr->name()));

      generateClientResourceStubHeader(service_ptr);
      generateClientResourceStubImp(service_ptr);

      generate_server_action_headers( service_ptr );
      generate_server_action_imp( service_ptr );

      generateServerResourceStubHeader(service_ptr);
      generateServerResourceStubImp(service_ptr);
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
      // printer.Print(mVars, "#include \"$service$Wrapper.h\"\n");
      for (int method_index = 0; method_index < pService->method_count(); method_index++)
      {
         const MethodDescriptor* method_ptr = pService->method( method_index );

         printer.Print("#include \"$service$$action$Action\"\n",
                       "action", method_ptr->name(),
                       "action_downcase", StringHelper::ToLower(method_ptr->name()),
                       "service", pService->name());
      }
      printer.Print("\n");

      // Add the service to the same namespace.
      printer.Print("namespace $package$ {\n", "package",
                  pService->file()->package());
      printer.Print("\n");

      // printer.Print(mVars, "struct $service$HookCb;\n");
      // printer.Print("\n");

      printer.Print(mVars, "class $service$ServerStub :\n"
                  "public liber::rpc::RpcServerResource {\n");
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
      printer.Outdent();
      printer.Print("private:\n\n");
      printer.Indent();

      for (int method_index = 0; method_index < pService->method_count(); method_index++)
      {
         const MethodDescriptor* method_ptr = pService->method( method_index );

         printer.Print("$service$$action$Action default_$action_downcase$_action_;\n",
                       "action", method_ptr->name(),
                       "action_downcase", StringHelper::ToLower(method_ptr->name()),
                       "service", pService->name());
      }

      // Close the resource definition.
      printer.Outdent();
      printer.Print(mVars, "\n}; // End $service$ServerStub\n");
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

      printer.Print(mVars, "$method_separator$");
      printer.Print(mVars, "$service$ServerStub::$service$ServerStub()\n");
      printer.Print(mVars, ": RpcServerResource(\"$service$\")\n");
      printer.Print("{\n");
      printer.Print("}\n");
      printer.Print("\n");

      printer.Print(mVars, "$method_separator$");
      printer.Print(mVars, "$service$ServerStub::~$service$ServerStub()\n");
      printer.Print("{\n");
      printer.Print("}\n");
      printer.Print("\n");

      printer.Print(mVars, "$method_separator$");
      printer.Print(mVars, "void $service$ServerStub::registerActions()\n");
      printer.Print("{\n");
      printer.Indent();
      for (int lnMethod = 0; lnMethod < pService->method_count(); lnMethod++)
      {
      const MethodDescriptor* lpMethod = pService->method(lnMethod);
      printer.Print("addAction( "
                    "&default_$method$_action_ );\n",
                    "method", lpMethod->name(),
                    "service", pService->name());
      }
      printer.Outdent();
      printer.Print("}\n");
      printer.Print("\n");

      lbFailed = printer.failed();
      delete pStream;
   }

   return (lbFailed == false); 
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::generate_server_action_headers(
   const ServiceDescriptor* service_ptr )
{
   bool success = false;

   for ( int method_index = 0;
         method_index < service_ptr->method_count();
         ++method_index )
   {
      const MethodDescriptor* method_ptr = service_ptr->method( method_index );

      std::string filename = service_ptr->name() +
                             method_ptr->name()  + "Action.h";

      // Open a stream to the output file.
      ZeroCopyOutputStream* stream = mpContext->Open( filename );

      if ( stream )
      {
         Printer printer( stream, PRINTER_DELIMETER );

         std::map<std::string,std::string> variables( mVars );
         variables[ "package" ] = service_ptr->file()->package();
         variables[ "service_upper" ] = StringHelper::ToUpper( service_ptr->name() );
         variables[ "action_upper" ] = StringHelper::ToUpper( method_ptr->name() );
         variables[ "service" ] = service_ptr->name();
         variables[ "action" ] = method_ptr->name();
         variables[ "request_type" ] = GetFullyQualifiedName( method_ptr->input_type() );
         variables[ "response_type" ] = GetFullyQualifiedName( method_ptr->output_type() );

         printer.Print(
            variables,
            "#ifndef $service_upper$_$action_upper$_ACTION_H\n"
            "#define $service_upper$_$action_upper$_ACTION_H\n"

            "#include \"RpcServerResource.h\"\n"
            "#include \"$package$.pb.h\"\n\n"

            "namespace $package$ {\n\n"

            "class $service$$action$Action : public liber::rpc::RpcServiceAction {\n"
            "public:\n\n"
            "   $service$Service$action$Action();\n\n"
            "   void operator() (\n"
            "      const std::string&        request,\n"
            "      std::string&              response,\n"
            "      liber::rpc::RpcException& e );\n\n"
            "protected:\n\n"
            "   virtual void $action$(\n"
            "      const $request_type$& request,\n"
            "      $response_type$& response,\n"
            "      liber::rpc::RpcException& e );\n"
            "};\n\n"
            "}  // End namespace $package$\n\n"
            "#endif // $service_upper$_$action_upper$_ACTION_H\n"
         );

         success = ( printer.failed() == false );
      }

      delete stream;
   }

   return success;
}

//-----------------------------------------------------------------------------
bool LiberFileGenerator::generate_server_action_imp(
  const ServiceDescriptor* service_ptr )
{
   bool success = false;

   for ( int method_index = 0;
         method_index < service_ptr->method_count();
         ++method_index )
   {
      const MethodDescriptor* method_ptr = service_ptr->method( method_index );

      std::string filename = service_ptr->name() + "" +
                             method_ptr->name()  + "Action.cpp";

      // Open a stream to the output file.
      ZeroCopyOutputStream* stream = mpContext->Open(filename);

      if ( stream )
      {
         Printer printer( stream, PRINTER_DELIMETER );

         std::map<std::string,std::string> variables( mVars );
         variables[ "package" ] = service_ptr->file()->package();
         variables[ "service" ] = service_ptr->name();
         variables[ "action" ] = method_ptr->name();
         variables[ "request_type" ] = GetFullyQualifiedName( method_ptr->input_type() );
         variables[ "response_type" ] = GetFullyQualifiedName( method_ptr->output_type() );

         printer.Print(
            variables,
            "#include \"$package$.h\"\n"
            "#include \"$service$Service$action$Action.h\"\n\n"
            "$method_separator$"
            "$service$$action$Action::$service$$action$Action()\n"
            "   : liber::rpc::RpcServiceAction( \"Search\" )\n"
            "{\n"
            "}\n\n"
            "$method_separator$"
            "void $service$$action$Action::operator() (\n"
            "   const std::string& request, std::string& response, RpcException& e )\n"
            "{\n"
            "   $request_type$ request_message;\n"
            "   $response_type$ response_message;\n\n"
            "   e.pushFrame( TraceFrame(\n"
            "      \"$service$$action$Action\",\n"
            "      \"$action$\",\n"
            "      __FILE__,\n"
            "      __LINE__\n"
            "   ));\n\n"
            "   request_message.ParseFromString( request );\n"
            "   if ( request_message.IsInitialized() )\n"
            "   {\n"
            "      $action$( request_message, response_message, e );\n"
            "      response_message.SerializeToString( &response );\n"
            "      e.popFrame();\n"
            "   }\n"
            "   else\n"
            "   {\n"
            "      e.id = MissingParameters;\n"
            "      e.message = \"Parameters not initialized\";\n"
            "   }\n"
            "}\n\n"
            "$method_separator$"
            "void $service$$action$Action::Search(\n"
            "   const $request_type$& request,\n"
            "   $response_type$& response,\n"
            "   liber::rpc::RpcException& e )\n"
            "{\n"
            "   e.id = UnimplementedAction;\n"
            "   e.message = \"$service$$action$Action has not been implemented.\";\n"
            "}\n"
         );

         success = ( printer.failed() == false );
      }

      delete stream;
   }

   return success;
}

//-----------------------------------------------------------------------------
std::string LiberFileGenerator::GetFullyQualifiedName(
   const Descriptor* descriptor_ptr )
{
   std::stringstream        full_name;
   std::vector<std::string> type_name_stack;

   const Descriptor* parent_descriptor_ptr = descriptor_ptr;
   while ( parent_descriptor_ptr != NULL )
   {
      type_name_stack.push_back( parent_descriptor_ptr->name() );
      parent_descriptor_ptr = parent_descriptor_ptr->containing_type();
   }

   while ( type_name_stack.empty() == false )
   {
      full_name << type_name_stack.back();
      type_name_stack.pop_back();

      if ( type_name_stack.empty() == false )
      {
         full_name << "_";
      }
   }

   return full_name.str();
} 

