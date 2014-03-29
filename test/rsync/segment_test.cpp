#include <fstream>
#include "SegmentList.h"
#include "Segmenter.h"
#include "SegmentHash.h"
#include "InstructionList.h"
#include "Authority.h"
#include "Assembler.h"
#include "SegmentFile.h"

using namespace liber::rsync;

int main (int argc, char* argv[])
{
  static const ui32 SegmentSize = 6;

  SegmentList   segments;
//  Segmenter     segmenter(SegmentSize, Segmenter::EveryOffset);
  Segmenter     segmenter(SegmentSize, Segmenter::FullStride);
  SegmentHash   hash;


  // Segment the client file.
  std::ifstream is("flowtestdata/client_dir/file.dat", std::ifstream::binary);
  if (!is.is_open())
  {
    std::cout << "Failed to open file." << std::endl;
    return -1;
  }

  segmenter.process(is, segments);
  is.close();

  std::vector<Segment*>::iterator segIt = segments.segments().begin();
  for (; segIt != segments.segments().end(); ++segIt)
  {
    std::cout << (*segIt)->debugDump() << std::endl;
    hash.insert(*segIt);
  }




  // Segment the authoritative file and create the instructions.
  JobDescriptor   descriptor("flowtestdata/server_dir/file.dat", SegmentSize);
  InstructionList ilist;
  Authority       authority(descriptor, hash, ilist);

  std::ifstream iauth("flowtestdata/server_dir/file.dat", std::ifstream::binary);
  if (!iauth.is_open())
  {
    std::cout << "Failed to open file." << std::endl;
    return -1;
  }

  authority.process(iauth);
  iauth.close();

  std::cout << std::endl << "Instructions (" << std::dec << ilist.instructions().size() << "):" << std::endl;
  std::vector<Instruction*>::iterator instruction = ilist.instructions().begin();

  for (; instruction != ilist.instructions().end(); ++instruction)
  {
    if (*instruction)
    {
      std::cout << (*instruction)->toString() << std::endl;
    }
    else
    {
      std::cout << "NULL instructions" << std::endl;
    }
  }



  // Reassemble the client file.
  std::ofstream reassemFile("flowtestdata/client_dir/reassembled_file.dat", std::ifstream::binary);

  JobDescriptor srcDesc("flowtestdata/client_dir/file.dat", SegmentSize);
  SegmentFile   accessor(srcDesc);
  Assembler     assembler(accessor, reassemFile);
  instruction = ilist.instructions().begin();
  for (; instruction != ilist.instructions().end(); ++instruction)
  {
    assembler.execute(*instruction);
  }



  return 0;
}

