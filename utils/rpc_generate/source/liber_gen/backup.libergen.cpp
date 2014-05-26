#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

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

    google::protobuf::io::ZeroCopyOutputStream* stream = generator_context->Open("test.cpp");

    int lnServiceCount = file->service_count();
    for (int lnService = 0; lnService < lnServiceCount; lnService++)
    {
      const google::protobuf::ServiceDescriptor* lpService = file->service(lnService);
      ofs << "Service:" << std::endl
          << lpService->DebugString() << std::endl << std::endl;

      std::string outstring = "hello, world";//lpService->DebugString();
      ofs << "writing " << outstring.size() << " bytes" << std::endl;
      streamString(ofs, stream, outstring);
    }

    delete stream;

    ofs.close();
    return true;
  }

  void streamString(std::ofstream& ofs, google::protobuf::io::ZeroCopyOutputStream* stream, const std::string& data) const
  {
    const char* lpData = data.c_str();
    int lnWritePos = 0;
    int lnStringLen = data.size();
    int lnRemaining = lnStringLen;

    while (lnWritePos < data.size())
    {
      char* lpOut = NULL;
      int lnBufferSize = 0;

      stream->Next((void**)&lpOut, &lnBufferSize);
      ofs << "Next: " << lnBufferSize << " bytes" << std::endl;
      if (lpOut && (lnBufferSize > 0))
      {
        memset(lpOut, 0, lnBufferSize);
        memcpy(lpOut, &lpData[lnWritePos], (lnBufferSize > lnRemaining) ? lnRemaining : lnBufferSize);
        lnWritePos += (lnBufferSize > lnRemaining) ? lnRemaining : lnBufferSize;

        if (lnBufferSize > lnRemaining) stream->BackUp(lnBufferSize - lnRemaining);
        lnRemaining = lnStringLen - lnWritePos;
      }
    }
  }
};

int main(int argc, char* argv[]) {
  LiberCodeGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}

