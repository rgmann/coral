#include <fstream>
#include "InstructionList.h"

using namespace liber::rsync;

void test_begin_instruction()
{
  std::cout << "test_begin_isntruction" << std::endl;

  JobDescriptor descriptor("flowtestdata/client_dir/file.dat", 6);

  BeginInstruction source(descriptor);
  std::cout << "before:" << std::endl
            << source.toString() << std::endl;
  std::string serialized = InstructionFactory::Serialize(&source);

  Instruction* lpDestination = InstructionFactory::Deserialize(serialized);
  if (lpDestination)
  {
    std::cout << "after:" << std::endl
              << lpDestination->toString() << std::endl;
    delete lpDestination;
    lpDestination = NULL;
  }
  else
  {
    std::cout << "Failed to deserialize." << std::endl;
  }
  std::cout << std::endl;
}


void test_segment_instruction()
{
  std::cout << "test_segment_instruction" << std::endl;

  SegmentInstruction source(15);
  std::cout << "before:" << std::endl
            << source.toString() << std::endl;
  std::string serialized = InstructionFactory::Serialize(&source);

  Instruction* lpDestination = InstructionFactory::Deserialize(serialized);
  if (lpDestination)
  {
    std::cout << "after:" << std::endl
              << lpDestination->toString() << std::endl;
    delete lpDestination;
    lpDestination = NULL;
  }
  else
  {
    std::cout << "Failed to deserialize." << std::endl;
  }
  std::cout << std::endl;
}


void test_chunk_instruction()
{
  ui8 buffer[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  std::cout << "test_chunk_instruction" << std::endl;

  ChunkInstruction source(sizeof(buffer));
  memcpy(source.data(), buffer, sizeof(buffer));
  std::cout << "before:" << std::endl
            << source.toString() << std::endl;
  std::string serialized = InstructionFactory::Serialize(&source);

  Instruction* lpDestination = InstructionFactory::Deserialize(serialized);
  if (lpDestination)
  {
    std::cout << "after:" << std::endl
              << lpDestination->toString() << std::endl;
    delete lpDestination;
    lpDestination = NULL;
  }
  else
  {
    std::cout << "Failed to deserialize." << std::endl;
  }
  std::cout << std::endl;
}


void test_end_instruction()
{
}


int main (int argc, char* argv[])
{
  test_begin_instruction();
  test_segment_instruction();
  test_chunk_instruction();
  return 0;
}

