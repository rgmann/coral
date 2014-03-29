#include "RsyncClient.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
bool RsyncClient::pull(const FilePath& source, const FilePath& destination)
{
  // Check that the source file exists.  Note: We don't know whether the
  // destination file exists, but that will be determined later on.
  RsyncCoreJob* lpJob = new RsyncCoreJob(RsyncPull, source, destination);

  return mJobQueue.push(lpJob, 100);
}

//-----------------------------------------------------------------------------
bool RsyncClient::push(const FilePath& source, const FilePath& destination)
{
  // Check that the source file exists.  Note: We don't know whether the
  // destination file exists, but that will be determined later on.

  RsyncJob* lpJob = new RsyncJob(RsyncPush, filename, source, destination);

  return mJobQueue.push(lpJob, 100);
}

//-----------------------------------------------------------------------------
void RsyncClient::TranslateThreadEntry(ThreadArg* pArg)
{
  ((RsyncClient*)pArg->pUserData)->translateThread(pArg);
}

//-----------------------------------------------------------------------------
void RsyncClient::translateThread(ThreadArg* pArg)
{
  RsyncPacket* lpPacket = NULL;

  while (!pArg->stopSignalled())
  {
    lpPacket = NULL;
    if (mRsyncOutQueue.pop(lpPacket, 100) && lpPacket)
    {
      sendPacket(lpPacket);
    }
  }
}

//-----------------------------------------------------------------------------
void RsyncJobThreadEntry(ThreadArg* pArg)
{
  ((RsyncClient*)pArg->pUserData)->rsyncJobThread(pArg);
}

//-----------------------------------------------------------------------------
void RsyncClient::rsyncJobThread(ThreadArg* pArg)
{
  RsyncJob* pActiveJob = NULL;

  FileQueryPacket lFileQuery;

  while (!pArg->stopSignalled())
  {
    // Get the next job.  If there are no jobs available, try again.
    if (!mJobQueue.pop(pActiveJob, 100))
    {
      continue;
    }


    // Handle a pull job.
    if (pActiveJob->type == RsyncPull)
    {
      // Setup a query to ask the server whether the destination file exists.
      lpRsyncPacket = pActiveJob->getFileQuery();

      // Send the "file exists?" query.
      if (lpRsyncPacket)
      {
        mRsyncOutQueue.push(lpRsyncPacket, 100);

        delete lpRsyncPacket;
        lpRsyncPacket = NULL;
      }
      else
      {
        printf("RsyncClient::rsyncJobThread: Failed to create file query!\n");
        continue;
      }


      // Wait for a response and attempt to unpack the response.
      if (!mInQueue.pop(lpRsyncPacket, 1000))
      {
        continue;
      }
      if (!lFileQueryResponse.unpack(lpRsyncePacket->dataPtr(), lpRsyncPacket->data()->length))
      {
        delete lpRsyncPacket;
        lpRsyncPacket = NULL;
        continue;
      }

      // If the server could not find the file, the job failed.
      if (!lFilequeryResponse.data()->fileExists)
      {
        delete lpRsyncPacket;
        lpRsyncPacket = NULL;
        continue;
      }

      if (pActiveJob->sendSegments(mRsyncOutQueue))
      {
        pActiveJob->reassemble(mInQueue);
      }
    }
    else if (pActiveJob->type == RsyncPush)
    {
      // The server will automatically create the file on the server if does
      // not exist yet.

      // Instruct the server to segment the specified file and reply with the
      // segments.
      lpRsyncPacket = pActiveJob->getFileRequest();
      // Send the "file exists?" query.
      if (lpRsyncPacket)
      {
        mRsyncOutQueue.push(lpRsyncPacket, 100);

        delete lpRsyncPacket;
        lpRsyncPacket = NULL;
      }
      else
      {
        printf("RsyncClient::rsyncJobThread: Failed to create file query!\n");
        continue;
      }

      // Receive the segment report from server.
      if (pActiveJob->receiveSegments(mInQueue))
      {
        // Use the file authority to create reassembly instructions for the server.
        pActiveJob->createInstructions(mRsyncOutQueue);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void RsyncClient::CallbackThreadEntry(ThreadArg* pArg)
{
  ((RsyncClient*)pArg->pUserData)->callbackThread(pArg);
}

//-----------------------------------------------------------------------------
void RsyncClient::callbackThread(ThreadArg* pArg)
{
  while (!pArg->stopSignalled())
  {
    if (mCompletedJobs.pop(lpJob, 100))
    {
      // If a callback has been registered, invoke it with the next completed
      // job's information.
      if (mpCompletionCallback)
      {
        mpCompletionCallback->call(lpJob->getJobInfo());
      }
    }
  }
}



