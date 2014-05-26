#ifndef JOB_REPORT_H
#define JOB_REPORT_H

#include <ostream>
#include "Timestamp.h"

namespace liber {
namespace rsync {

struct SegmentationReport {

  SegmentationReport()
  {
    segmentSizeBytes = 0;
    strideSizeBytes = 0;
    segmentCount = 0;
  }

  Timestamp begin;
  Timestamp end;

  ui32 segmentSizeBytes;
  ui32 strideSizeBytes;

  ui32 segmentCount;

  void print(std::ostream&, ui8) const;
};

struct AssemblyReport {

  AssemblyReport()
  {
    segmentCount = 0;
    chunkCount   = 0;
  }

  ui32 segmentCount;
  ui32 chunkCount;

  Timestamp begin;
  Timestamp end;

  void print(std::ostream&, ui8) const;
};

struct AuthorityReport {

  AuthorityReport()
  {
    receivedSegmentCount = 0;
    matchedSegmentCount = 0;
    chunkCount = 0;
  }

  ui32 receivedSegmentCount;
  ui32 matchedSegmentCount;

  ui32 chunkCount;

  Timestamp hashBegin;
  Timestamp hashEnd;

  Timestamp authBegin;
  Timestamp authEnd;

  void print(std::ostream&, ui8) const;
};

class JobReport {
public:

  JobReport();

  // Destination file segmentation report.
  struct DestinationReport {
    SegmentationReport segmentation;
    AssemblyReport     assembly;
  };
  inline DestinationReport& destination() { return mDestinationReport; };

  struct SourceReport {
    SegmentationReport segmentation;
    AuthorityReport   authority;
  };
  inline SourceReport& source() { return mSourceReport; };


  void print(std::ostream&) const;

private:

  DestinationReport mDestinationReport;
  SourceReport      mSourceReport;
};

} // End namespace rsync
} // End namespace liber

#endif // JOB_REPORT_H
