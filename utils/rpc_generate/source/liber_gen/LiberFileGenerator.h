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

class LiberFileGenerator {
public:

  LiberFileGenerator(std::ofstream& debug,
                     const google::protobuf::FileDescriptor* pDescriptor,
                     google::protobuf::compiler::GeneratorContext* pContext);
  ~LiberFileGenerator();

  bool generateServices();

private:

  bool generate_client_resource_stub_header(
              const google::protobuf::ServiceDescriptor* pDescriptor);

  bool generate_client_resource_stub_imp(
              const google::protobuf::ServiceDescriptor* pDescriptor);

  bool generate_server_resource_stub_header(
              const google::protobuf::ServiceDescriptor* pDescriptor);

  bool generate_server_resource_stub_imp(
              const google::protobuf::ServiceDescriptor* pDescriptor);

  bool generate_server_action_headers(
    const google::protobuf::ServiceDescriptor* );
  bool generate_server_action_imp(
    const google::protobuf::ServiceDescriptor* );

  static std::string GetFullyQualifiedName(
    const google::protobuf::Descriptor* );

private:

  std::ofstream& mDebug;
  const google::protobuf::FileDescriptor*       mpDescriptor; 
  google::protobuf::compiler::GeneratorContext* mpContext;
  std::map<std::string, std::string> common_variables_;
};

#endif // LIBER_FILE_GENERATOR_H
