#ifndef ASSEMBLER_UNIT_H
#define ASSEMBLER_UNIT_H

#include <string>
#include "WorkUnit.h"
#include "RsyncSegmenter.h"
#include "RsyncAssembler.h"

namespace liber {
namespace rsync {

class AssemblerUnit : public WorkUnit {
public:
   
  AssemblerUnit(Queue<RsyncPacket*>& outQueue,
                RsyncSegmenter* pSegmenter = NULL);
   
  ~AssemblerUnit();

  void setSegmenter(RsyncSegmenter* pSegmenter);

  virtual bool work();

private:

  RsyncSegmenter* m_pSegmenter;

  RsyncAssembler  m_assembler;
};

}}

#endif // ASSEMBLER_UNIT_H
