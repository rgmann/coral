#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include "LiberFileGenerator.h"

using namespace google::protobuf::compiler;

class LiberCodeGenerator : public cpp::CppGenerator {
public:

  LiberCodeGenerator() : cpp::CppGenerator() {};
  ~LiberCodeGenerator() {};

  virtual bool Generate(const google::protobuf::FileDescriptor* file,
                        const std::string& parameter, 
                        google::protobuf::compiler::GeneratorContext* generator_context, 
                        std::string * error) const
  {
    std::ofstream ofs("/Users/vaughanbiker/Development/liber/utils/rpc_generate/source/liber_gen/message.txt");
    ofs << "LiberCodeGenerator:" << std::endl
        << "  name:    " << file->name() << std::endl
        << "  package: " << file->package() << std::endl << std::endl;

    LiberFileGenerator generator(ofs, file, generator_context);

    generator.generateServices();

    ofs.close();
    return true;
  }
};

int main(int argc, char* argv[]) {
  LiberCodeGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}

