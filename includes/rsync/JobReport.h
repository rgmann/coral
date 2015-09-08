#ifndef JOB_REPORT_H
#define JOB_REPORT_H

#include <ostream>
#include "Timestamp.h"
#include "Serializable.h"
#include "RsyncError.h"

namespace liber {
namespace rsync {

class SegmentationReport : public liber::netapp::Serializable {
public:
  SegmentationReport();

  RsyncError status;
  bool complete;

  Timestamp begin;
  Timestamp end;

  ui32 segmentSizeBytes;
  ui32 strideSizeBytes;

  ui32 segmentCount;

  void print(std::ostream&, ui8) const;

protected:

  void pack(liber::netapp::SerialStream&);
  void pack(liber::netapp::SerialStream&) const;
  bool unpack(liber::netapp::SerialStream&);
};

class AssemblyReport : public liber::netapp::Serializable {
public:
  AssemblyReport();

  RsyncError status;
  bool complete;

  ui32 segmentCount;
  ui32 chunkCount;

  Timestamp begin;
  Timestamp end;

  void print(std::ostream&, ui8) const;

protected:

  void pack(liber::netapp::SerialStream&);
  void pack(liber::netapp::SerialStream&) const;
  bool unpack(liber::netapp::SerialStream&);
};

class AuthorityReport : public liber::netapp::Serializable {
public:
  AuthorityReport();

  RsyncError status;
  bool complete;

  ui32 receivedSegmentCount;
  ui32 matchedSegmentCount;

  ui32 chunkCount;

  Timestamp hashBegin;
  Timestamp hashEnd;

  Timestamp authBegin;
  Timestamp authEnd;

  void print(std::ostream&, ui8) const;

protected:

  void pack(liber::netapp::SerialStream& ctor);
  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);
};

class DestinationReport : public liber::netapp::Serializable {
public:

  SegmentationReport segmentation;
  AssemblyReport     assembly;

protected:

  void pack(liber::netapp::SerialStream& ctor);
  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);
};

class SourceReport : public liber::netapp::Serializable {
public:

  SegmentationReport segmentation;
  AuthorityReport   authority;

protected:

  void pack(liber::netapp::SerialStream& ctor);
  void pack(liber::netapp::SerialStream& ctor) const;
  bool unpack(liber::netapp::SerialStream& dtor);
};

class JobReport : public liber::netapp::Serializable {
public:

  JobReport();

  RsyncError createJobStatus;

  // Destination file segmentation report.
  DestinationReport destination;
  SourceReport      source;


  /**
   * Indicates whether job was completed successfully.
   */
  bool success() const;
  void print(std::ostream&) const;
  std::string toString() const;

protected:

  void pack(liber::netapp::SerialStream&);
  void pack(liber::netapp::SerialStream&) const;
  bool unpack(liber::netapp::SerialStream&);
};

} // End namespace rsync
} // End namespace liber

#endif // JOB_REPORT_H
