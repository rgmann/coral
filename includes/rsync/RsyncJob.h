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


#ifndef RSYNC_JOB_H
#define RSYNC_JOB_H

#include <boost/filesystem.hpp>
#include "BinarySem.h"
#include "JobDescriptor.h"
#include "JobReport.h"
#include "SegmentQueue.h"
#include "InstructionQueue.h"

namespace coral {
namespace rsync {

class FileSystemInterface;
class RsyncPacketRouter;
class RsyncJobCallback;

class RsyncJob {
public:

  RsyncJob( FileSystemInterface& file_sys_interface, RsyncPacketRouter& router );
  ~RsyncJob();

  void setDescriptor( const JobDescriptor& descriptor );
  JobDescriptor& descriptor();

  JobReport& report();
  void mergeReport(const JobReport& rReport);

  SegmentQueue& segments();

  InstructionQueue& instructions();

  void signalSegmentationDone();
  void signalAuthDone();
  void signalAssemblyDone();
  void signalAllDone();

  bool done() const;
  bool waitDone(int nTimeoutMs = coral::thread::Semaphore::SemWaitForever);

  RsyncPacketRouter& packetRouter(){ return router_; };

  FileSystemInterface& fileSystem(){ return file_sys_interface_; };

private:
 
  RsyncJob(const RsyncJob&);
  RsyncJob& operator= (const RsyncJob&);

private:

  JobDescriptor mDescriptor;

  RsyncPacketRouter& router_;
  FileSystemInterface& file_sys_interface_;

  JobReport report_;

  SegmentQueue mSegments;
  InstructionQueue mInstructions;

  thread::BinarySem mSegmentationDone;
  thread::BinarySem mAuthDone;
  thread::BinarySem mAssemblyDone;
};

} // End namespace rsync
} // End namespace coral

#endif // RSYNC_JOB_H
