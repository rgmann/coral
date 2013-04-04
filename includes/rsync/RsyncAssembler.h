#ifndef RSYNC_ASSEMBLER_H
#define RSYNC_ASSEMBLER_H

#include "RsyncFileAuthority.h"
#include "RsyncSegmenter.h"
#include "RsyncFileStage.h"
#include <vector>


class RsyncAssembler
{
public:
   
   enum RsyncAssemblyMode
   {
      NoStaging,
      StageInstructions
   };
   
   enum AddStatus
   {
      Failure,
      AddOk,
      BeginMarker,
      EndMarker
   };
   
   RsyncAssembler();
   
   ~RsyncAssembler();
   
   void  setSegmenter(RsyncSegmenter* pSegmenter);
   
   //bool  start(RsyncAssemblySeqMarker *pMarker, RsyncAssemblyMode mode);
   void start(RsyncAssemblyMode mode);
   
   AddStatus  addInstruction(void* pInstr,
                        unsigned int nInstrLenBytes);
   
   bool  assemble();
   
   bool  reset();
   
private:
   
   RsyncAssembler(const RsyncAssembler& other);
   
   RsyncAssembler& operator= (const RsyncAssembler& other);
   
   bool execAssemblyInstr(RsyncAssemblyInstr* pInstr);
   
private:
   
   RsyncSegmenter* m_pSegmenter;
   
   RsyncAssemblySeqMarker*  m_pAssemblyMarker;
   
   RsyncAssemblyMode    m_eAssemblyMode;
   
   std::vector<RsyncAssemblyInstr*> m_vAssmbInstrs;
   
   RsyncSegVec          m_vSegments;
   
   RsyncFileStage*   m_pStage;
};


#endif // RSYNC_ASSEMBLER_H