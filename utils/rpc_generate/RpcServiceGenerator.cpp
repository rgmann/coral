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

#include <map>
#include <vector>
#include <utility>
#include <sstream>
#include <boost/scoped_ptr.hpp>
#include <google/protobuf/io/printer.h>
#include "StringHelper.h"
#include "RpcServiceGenerator.h"

#define PRINTER_DELIMETER        ('$')

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::compiler;

//-----------------------------------------------------------------------------
RpcServiceGenerator::RpcServiceGenerator(
   std::ofstream&        debug,
   const FileDescriptor* descriptor,
   GeneratorContext*     context
)
   : debug_stream_( debug )
   , descriptor_( descriptor )
   , context_( context )
{
}

//-----------------------------------------------------------------------------
RpcServiceGenerator::~RpcServiceGenerator()
{
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generateServices()
{
   bool success = true;

   common_variables_.insert( std::make_pair(
      "method_separator",
      "//-----------------------------------------------------------------------------\n"
   ));

   common_variables_.insert( std::make_pair(
      "license_token",
      "// %% license-end-token %%"
   ));

   common_variables_.insert( std::make_pair(
      "autogen_notice",
      "//\n"
      "// This file was generated by Coral RPC.\n"
      "//\n"
      "// WARNING: Auto-generated code. Do not modify.\n"
      "//\n"
   ));

   for ( int service_index = 0;
         service_index < descriptor_->service_count();
         ++service_index )
   {
      const ServiceDescriptor* service_ptr = NULL;

      service_ptr = descriptor_->service( service_index );

      common_variables_.insert( std::make_pair(
         "service",
         service_ptr->name()
      ));

      generate_client_resource_stub_header( service_ptr );
      generate_client_resource_stub_imp( service_ptr );

      generate_server_action_headers( service_ptr );
      generate_server_action_imp( service_ptr );

      generate_server_resource_stub_header( service_ptr );
      generate_server_resource_stub_imp( service_ptr );

      common_variables_.erase( "service" );
   }

   return success;
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generate_client_resource_stub_header(
   const ServiceDescriptor* service_ptr )
{
   bool success = false;

   std::string filename = service_ptr->name() + "ClientStub.h";

   // Open a stream to the output file.
   boost::scoped_ptr<ZeroCopyOutputStream> stream( context_->Open( filename ) );

   if ( stream )
   {
      Printer printer( stream.get(), PRINTER_DELIMETER );

      debug_stream_ << "RpcServiceGenerator: Generating " << filename << "." << std::endl;

      std::map<std::string,std::string> variables( common_variables_ );
      variables[ "package" ]       = service_ptr->file()->package();
      variables[ "service_upper" ] = StringHelper::ToUpper( service_ptr->name() );
      variables[ "service" ]       = service_ptr->name();

      printer.Print(
         variables,
         "$license_token$\n"
         "\n"
         "$autogen_notice$"
         "\n"
         "#ifndef $service_upper$_CLIENT_STUB_H\n"
         "#define $service_upper$_CLIENT_STUB_H\n"
         "\n"
         "#include \"RpcClientResource.h\"\n"
         "#include \"$package$.pb.h\"\n"
         "\n"
         "namespace $package$ {\n"
         "\n"
         "class  $service$ClientStub :\n"
         "public coral::rpc::RpcClientResource {\n"
         "public:\n"
         "\n"
         "   explicit $service$ClientStub( coral::rpc::RpcClient &client );\n"
         "   ~$service$ClientStub();\n"
         "\n"
      );

      int method_count = service_ptr->method_count();
      for ( int method_index = 0; method_index < method_count; ++method_index )
      {
         const MethodDescriptor* method_ptr = service_ptr->method( method_index );

         std::map<std::string,std::string> local_variables( variables );
         variables[ "method" ] = method_ptr->name();
         variables[ "request_type" ] =
            GetFullyQualifiedName( method_ptr->input_type() );
         variables[ "response_type" ] =
            GetFullyQualifiedName( method_ptr->output_type() );

         printer.Print(
            variables,
            "   void $method$(\n"
            "      const $request_type$& request,\n"
            "      $response_type$& response,\n"
            "      coral::rpc::AsyncRpcSupervisor* supervisor = NULL )"
            " throw (coral::rpc::RpcException);\n\n"
         );
      }

      printer.Print(
         variables,
         "}; // End $service$ClientStub\n"
         "\n"
         "}  // End namespace $package$\n"
         "\n"
         "#endif // $service_upper$_CLIENT_STUB_H\n"
      );

      success = ( printer.failed() == false );
   }

   return success; 
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generate_client_resource_stub_imp(
   const ServiceDescriptor* service_ptr )
{
   bool success = false;

   std::string filename = service_ptr->name() + "ClientStub.cpp";

   // Open a stream to the output file.
   boost::scoped_ptr<ZeroCopyOutputStream> stream( context_->Open( filename ) );

   if ( stream )
   {
      Printer printer( stream.get(), PRINTER_DELIMETER );

      debug_stream_ << "RpcServiceGenerator: Generating " << filename
                    << "." << std::endl;

      std::map<std::string,std::string> variables( common_variables_ );
      variables[ "package" ]       = service_ptr->file()->package();
      variables[ "service_upper" ] = StringHelper::ToUpper( service_ptr->name() );
      variables[ "service" ]       = service_ptr->name();

      printer.Print(
         variables,
         "$license_token$\n"
         "\n"
         "$autogen_notice$"
         "\n"
         "#include \"$service$ClientStub.h\"\n"
         "\n"
         "using namespace coral::rpc;\n"
         "using namespace $package$;\n"
         "\n"
         "$method_separator$"
         "$service$ClientStub::$service$ClientStub( RpcClient &client )\n"
         "   : RpcClientResource( client, \"$service$\" )\n"
         "{\n"
         "}\n"
         "\n"
         "$method_separator$"
         "$service$ClientStub::~$service$ClientStub()\n"
         "{\n"
         "}\n"
         "\n"
      );

      int method_count = service_ptr->method_count();
      for ( int method_index = 0; method_index < method_count; ++method_index )
      {
         const MethodDescriptor* method_ptr = service_ptr->method( method_index );

         std::map<std::string,std::string> local_variables( variables );
         local_variables[ "method" ]   = method_ptr->name();
         local_variables[ "request" ]  = GetFullyQualifiedName(method_ptr->input_type());
         local_variables[ "response" ] = GetFullyQualifiedName(method_ptr->output_type());

         printer.Print(
            local_variables,
            "$method_separator$"
            "void $service$ClientStub::$method$(\n"
            "   const $request$&    request,\n"
            "   $response$&         response,\n"
            "   AsyncRpcSupervisor* supervisor )\n"
            "{\n"
            "   if ( call( \"$method$\", request, response, supervisor ) == false )\n"
            "   {\n"
            "      throw getLastError();\n"
            "   }\n"
            "}\n"
            "\n"
         );
      }
      printer.Print("\n");

      success = ( printer.failed() == false );
   }

   return success; 
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generate_server_resource_stub_header(
   const ServiceDescriptor* service_ptr )
{
   bool success = false;

   std::string filename = service_ptr->name() + "ServerStub.h";

   // Open a stream to the output file.
   boost::scoped_ptr<ZeroCopyOutputStream> stream( context_->Open( filename ) );

   if ( stream )
   {
      Printer printer( stream.get(), PRINTER_DELIMETER );
      debug_stream_ << "RpcServiceGenerator: Generating " << filename << "." << std::endl;

      std::map<std::string,std::string> variables( common_variables_ );
      variables[ "package" ]       = service_ptr->file()->package();
      variables[ "service_upper" ] = StringHelper::ToUpper( service_ptr->name() );
      variables[ "service" ]       = service_ptr->name();

      printer.Print(
         variables,
         "$license_token$\n"
         "\n"
         "$autogen_notice$"
         "\n"
         "#ifndef $service_upper$_SERVER_STUB_H\n"
         "#define $service_upper$_SERVER_STUB_H\n"
         "\n"
         "#include \"RpcServerResource.h\"\n"
         "\n"
      );


      // Includes
      for ( int method_index = 0; method_index < service_ptr->method_count(); ++method_index )
      {
         const MethodDescriptor* method_ptr = service_ptr->method( method_index );

         std::map<std::string,std::string> local_variables( variables );
         local_variables[ "action" ] = method_ptr->name();
         local_variables[ "action_downcase" ] = StringHelper::ToLower(method_ptr->name());
         local_variables[ "service" ] = service_ptr->name();

         printer.Print( local_variables, "#include \"$service$$action$Action.h\"\n" );
      }

      // Add the service to the same namespace.
      printer.Print(
         variables,
         "\n"
         "namespace $package$ {\n"
         "\n"
         "class $service$ServerStub : public coral::rpc::RpcServerResource {\n"
         "public:\n"
         "\n"
         "   explicit $service$ServerStub();\n"
         "   ~$service$ServerStub();\n"
         "\n"
         "   virtual void registerActions();\n"
         "\n"
         "\n"
         "private:\n"
         "\n"
      );

      for ( int method_index = 0; method_index < service_ptr->method_count(); ++method_index )
      {
         const MethodDescriptor* method_ptr = service_ptr->method( method_index );

         std::map<std::string,std::string> local_variables( variables );
         local_variables[ "action" ] = method_ptr->name();
         local_variables[ "action_downcase" ] = StringHelper::ToLower(method_ptr->name());
         local_variables[ "service" ] = service_ptr->name();

         printer.Print(
            local_variables,
            "   $service$$action$Action default_$action_downcase$_action_;\n"
         );
      }

      // Close the resource definition.
      printer.Print(
         variables,
         "\n"
         "}; // End $service$ServerStub\n"
         "\n"
         "}  // End namespace $package$\n"
         "\n"
         "\n#endif // $service_upper$_SERVER_STUB_H\n"
      );

      success = ( printer.failed() == false );

   }

   return success; 
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generate_server_resource_stub_imp(
   const ServiceDescriptor* service_ptr )
{
   bool success = false;

   std::string filename = service_ptr->name() + "ServerStub.cpp";

   // Open a stream to the output file.
   boost::scoped_ptr<ZeroCopyOutputStream> stream( context_->Open( filename ) );

   if ( stream )
   {
      Printer printer( stream.get(), PRINTER_DELIMETER );
      debug_stream_ << "RpcServiceGenerator: Generating " << filename << "." << std::endl;

      std::map<std::string,std::string> variables( common_variables_ );
      variables[ "package" ]       = service_ptr->file()->package();
      variables[ "service_upper" ] = StringHelper::ToUpper( service_ptr->name() );
      variables[ "service" ]       = service_ptr->name();

      printer.Print(
         variables,
         "$license_token$\n"
         "\n"
         "$autogen_notice$"
         "\n"
         "#include \"$service$ServerStub.h\"\n"
         "\n"
         "using namespace coral::rpc;\n"
         "using namespace $package$;\n"
         "\n"
         "$method_separator$"
         "$service$ServerStub::$service$ServerStub()\n"
         "   : RpcServerResource(\"$service$\")\n"
         "{\n"
         "}\n"
         "\n"
         "$method_separator$"
         "$service$ServerStub::~$service$ServerStub()\n"
         "{\n"
         "}\n"
         "\n"
         "$method_separator$"
         "void $service$ServerStub::registerActions()\n"
         "{\n"
      );

      for ( int method_index = 0; method_index < service_ptr->method_count(); ++method_index )
      {
         const MethodDescriptor* method_ptr = service_ptr->method( method_index );

         std::map<std::string,std::string> local_variables( variables );
         local_variables[ "action" ] = method_ptr->name();
         local_variables[ "service" ] = service_ptr->name();

         printer.Print(
            local_variables,
            "   addAction( &default_$action$_action_ );\n" );
      }

      printer.Print(
         variables,
         "}\n"
         "\n"
      );

      success = ( printer.failed() == false );
   }

   return success; 
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generate_server_action_headers(
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
      boost::scoped_ptr<ZeroCopyOutputStream> stream( context_->Open( filename ) );

      if ( stream )
      {
         Printer printer( stream.get(), PRINTER_DELIMETER );

         std::map<std::string,std::string> variables( common_variables_ );
         variables[ "package" ]       = service_ptr->file()->package();
         variables[ "service_upper" ] = StringHelper::ToUpper( service_ptr->name() );
         variables[ "action_upper" ]  = StringHelper::ToUpper( method_ptr->name() );
         variables[ "service" ]       = service_ptr->name();
         variables[ "action" ]        = method_ptr->name();
         variables[ "request_type" ]  = GetFullyQualifiedName( method_ptr->input_type() );
         variables[ "response_type" ] = GetFullyQualifiedName( method_ptr->output_type() );

         printer.Print(
            variables,
            "$license_token$\n"
            "\n"
            "$autogen_notice$"
            "\n"
            "#ifndef $service_upper$_$action_upper$_ACTION_H\n"
            "#define $service_upper$_$action_upper$_ACTION_H\n"
            "\n"
            "#include \"RpcServerResource.h\"\n"
            "#include \"$package$.pb.h\"\n"
            "\n"
            "namespace $package$ {\n"
            "\n"
            "class $service$$action$Action : public coral::rpc::RpcServiceAction {\n"
            "public:\n\n"
            "   $service$$action$Action();\n"
            "\n"
            "   void operator() (\n"
            "      const std::string&        request,\n"
            "      std::string&              response,\n"
            "      coral::rpc::RpcException& e );\n"
            "\n"
            "protected:\n"
            "\n"
            "   virtual void $action$(\n"
            "      const $request_type$& request,\n"
            "      $response_type$& response,\n"
            "      coral::rpc::RpcException& e );\n"
            "\n"
            "};\n"
            "\n"
            "}  // End namespace $package$\n"
            "\n"
            "#endif // $service_upper$_$action_upper$_ACTION_H\n"
         );

         success = ( printer.failed() == false );
      }
   }

   return success;
}

//-----------------------------------------------------------------------------
bool RpcServiceGenerator::generate_server_action_imp(
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
      boost::scoped_ptr<ZeroCopyOutputStream> stream( context_->Open( filename ) );

      if ( stream )
      {
         Printer printer( stream.get(), PRINTER_DELIMETER );

         std::map<std::string,std::string> variables( common_variables_ );

         variables[ "package" ] = service_ptr->file()->package();
         variables[ "service" ] = service_ptr->name();
         variables[ "action" ]  = method_ptr->name();
         variables[ "request_type" ]  = GetFullyQualifiedName( method_ptr->input_type() );
         variables[ "response_type" ] = GetFullyQualifiedName( method_ptr->output_type() );

         printer.Print(
            variables,
            "$license_token$\n"
            "\n"
            "$autogen_notice$"
            "\n"
            "#include \"$package$.pb.h\"\n"
            "#include \"$service$$action$Action.h\"\n\n"
            "using namespace coral::rpc;\n"
            "using namespace $package$;\n\n"
            "$method_separator$"
            "$service$$action$Action::$service$$action$Action()\n"
            "   : coral::rpc::RpcServiceAction( \"$action$\" )\n"
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
            "void $service$$action$Action::$action$(\n"
            "   const $request_type$& request,\n"
            "   $response_type$& response,\n"
            "   coral::rpc::RpcException& e )\n"
            "{\n"
            "   e.id = UnimplementedAction;\n"
            "   e.message = \"$service$$action$Action has not been implemented.\";\n"
            "}\n"
         );

         success = ( printer.failed() == false );
      }
   }

   return success;
}

//-----------------------------------------------------------------------------
std::string RpcServiceGenerator::GetFullyQualifiedName(
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

