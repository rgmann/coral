#ifndef JOB_REPORT_H
#define JOB_REPORT_H

#include <ostream>
#include "Timestamp.h"
#include "PacketHelper.h"
#include "RsyncError.h"

namespace liber {
namespace rsync {

class SegmentationReport : public liber::netapp::Serializable {
public:
  SegmentationReport();

  RsyncError status;

  Timestamp begin;
  Timestamp end;

  ui32 segmentSizeBytes;
  ui32 strideSizeBytes;

  ui32 segmentCount;

  void print(std::ostream&, ui8) const;

protected:

  void pack(liber::netapp::PacketCtor&);
  void pack(liber::netapp::PacketCtor&) const;
  bool unpack(liber::netapp::PacketDtor&);
};

class AssemblyReport : public liber::netapp::Serializable {
public:
  AssemblyReport();

  RsyncError status;

  ui32 segmentCount;
  ui32 chunkCount;

  Timestamp begin;
  Timestamp end;

  void print(std::ostream&, ui8) const;

protected:

  void pack(liber::netapp::PacketCtor&);
  void pack(liber::netapp::PacketCtor&) const;
  bool unpack(liber::netapp::PacketDtor&);
};

class AuthorityReport : public liber::netapp::Serializable {
public:
  AuthorityReport();

  RsyncError status;

  ui32 receivedSegmentCount;
  ui32 matchedSegmentCount;

  ui32 chunkCount;

  Timestamp hashBegin;
  Timestamp hashEnd;

  Timestamp authBegin;
  Timestamp authEnd;

  void print(std::ostream&, ui8) const;

protected:

  void pack(liber::netapp::PacketCtor& ctor);
  void pack(liber::netapp::PacketCtor& ctor) const;
  bool unpack(liber::netapp::PacketDtor& dtor);
};

class JobReport : public liber::netapp::Serializable {
public:

  JobReport();

  RsyncError createJobStatus;

  // Destination file segmentation report.
  struct DestinationReport {
    SegmentationReport segmentation;
    AssemblyReport     assembly;
  } destination;

  struct SourceReport {
    SegmentationReport segmentation;
    AuthorityReport   authority;
  } source;


  /**
   * Indicates whether job was completed successfully.
   */
  bool success() const;
  void print(std::ostream&) const;

protected:

  void pack(liber::netapp::PacketCtor&);
  void pack(liber::netapp::PacketCtor&) const;
  bool unpack(liber::netapp::PacketDtor&);
};

} // End namespace rsync
} // End namespace liber

#endif // JOB_REPORT_H
