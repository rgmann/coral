#include "Log.h"
#include "RsyncJob.h"
#include "SegmentFile.h"
#include "JobDescriptor.h"
#include "FileSystemInterface.h"
#include "AssemblerThread.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AssemblerThread::AssemblerThread( FileSystemInterface& file_sys_interface )
: IThread("AssemblerThread")
, file_sys_interface_ ( file_sys_interface )
, segment_file_       ( file_sys_interface )
, assembler_          ( segment_file_ )
{
  job_queue_.initialize();
}

//----------------------------------------------------------------------------
AssemblerThread::~AssemblerThread()
{
}

//----------------------------------------------------------------------------
void AssemblerThread::addJob( RsyncJob* job_ptr )
{
  job_queue_.push( job_ptr );
}

//----------------------------------------------------------------------------
void AssemblerThread::run(const bool& bShutdown)
{
  boost::filesystem::path stage_path_;

  while (!bShutdown)
  {
    RsyncJob* job_ptr = NULL;

    if ( job_queue_.pop( job_ptr) && job_ptr )
    {
      if ( segment_file_.open( job_ptr->descriptor() ) )
      {
        bool stage_success = file_sys_interface_.stage(
          job_ptr->descriptor().getDestination().path,
          stage_path_,
          assembler_.outputStream()
        );

        if ( stage_success )
        {
          bool assembly_success = assembler_.process(
            job_ptr->instructions(),
            job_ptr->report().destination.assembly
          );

          if ( assembly_success == false )
          {
            log::error(
              "AssemblerThread - Assembly failed for %s: %s\n",
              job_ptr->descriptor().getDestination().path.string().c_str(),
              ExecutionStatus::ErrorDescription(
                assembler_.status().error
              ).c_str()
            );
          }

          // Close the segment file.
          segment_file_.close();

          // If assembly completed successfully,
          // swap the stage file and destination file.
          if ( assembly_success )
          {
            bool swap_success = file_sys_interface_.swap(
              stage_path_,
              job_ptr->descriptor().getDestination().path
            );

            if ( swap_success == false )
            {
              log::error("AssemblerThread - "
                         "Failed to swap stage file and destination file.\n");
            }
          }
        }
        else
        {
          segment_file_.close();

          log::error(
            "AssemblerThread - Failed to open stage file for %s\n",
            job_ptr->descriptor().getDestination().path.string().c_str());
        }
      }
      else
      {
        log::error("AssemblerThread - Failed to open segment access file.\n");
      }

      // When assembly is complete (regardless of whether is was successful),
      // signal that the RSYNC job is done.
      job_ptr->signalAssemblyDone();
    }
  }
}

