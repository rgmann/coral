#include "RsyncJobCallback.h"
#include "Worker.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
Worker::Worker(
   JobAgentPairQueue& segment_queue,
   JobAgentPairQueue& local_auth_queue,
   JobAgentPairQueue& assmebly_queue,
   JobAgentPairQueue& remote_auth_queue
)
:  segmenter_        ( segment_queue )
,  authority_        ( local_auth_queue )
,  assembler_        ( assmebly_queue )
,  remote_authority_ ( remote_auth_queue )
{
   segmenter_.launch();
   authority_.launch();
   assembler_.launch();
   remote_authority_.launch();
}

//-----------------------------------------------------------------------------
Worker::~Worker()
{
   segmenter_.cancel( true );
   authority_.cancel( true );
   assembler_.cancel( true );
   remote_authority_.cancel( true );
}
