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

#ifndef LIBER_FILE_GENERATOR_H
#define LIBER_FILE_GENERATOR_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

class RpcServiceGenerator {
public:

   RpcServiceGenerator(
      std::ofstream& debug,
      const google::protobuf::FileDescriptor*       descriptor,
      google::protobuf::compiler::GeneratorContext* context );

   ~RpcServiceGenerator();

   bool generateServices();


private:

   bool generate_client_resource_stub_header(
      const google::protobuf::ServiceDescriptor* );

   bool generate_client_resource_stub_imp(
      const google::protobuf::ServiceDescriptor* );

   bool generate_server_resource_stub_header(
      const google::protobuf::ServiceDescriptor* );

   bool generate_server_resource_stub_imp(
      const google::protobuf::ServiceDescriptor* );

   bool generate_server_action_headers(
      const google::protobuf::ServiceDescriptor* );

   bool generate_server_action_imp(
      const google::protobuf::ServiceDescriptor* );

   static std::string GetFullyQualifiedName(
      const google::protobuf::Descriptor* );


private:

   std::ofstream& debug_stream_;
   const google::protobuf::FileDescriptor*       descriptor_; 
   google::protobuf::compiler::GeneratorContext* context_;
   std::map<std::string, std::string>            common_variables_;
};

#endif // LIBER_FILE_GENERATOR_H
