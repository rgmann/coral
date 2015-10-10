// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include "RpcServiceGenerator.h"

using namespace google::protobuf::compiler;

class RpcGenerator : public cpp::CppGenerator {
public:

   RpcGenerator()
   {
   }

   ~RpcGenerator()
   {
   }

   virtual bool Generate(
      const google::protobuf::FileDescriptor* descriptor,
      const std::string&   parameter, 
      GeneratorContext*    generator_context, 
      std::string*         error ) const
   {
      std::ofstream debug_stream;

      // TODO: Open debug log file, DEBUG_FILE, if --debug_out=DEBUG_FILE is
      // specified at the command line.

      if ( debug_stream.is_open() )
      {
         debug_stream << "Generating package: " << descriptor->package()
                      << " from " << descriptor->name() << "\n";
      }

      RpcServiceGenerator generator( debug_stream, descriptor, generator_context );

      generator.generateServices();

      if ( debug_stream.is_open() )
      {
         debug_stream.close();
      }

      return error->empty();
   }
};

int main( int argc, char* argv[] )
{
   RpcGenerator generator;
   return google::protobuf::compiler::PluginMain( argc, argv, &generator );
}

