// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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
