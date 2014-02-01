#ifndef RSYNC_JOB_H
#define RSYNC_JOB_H



class RsyncJob {
public:

  enum Type {
    Push = 0,
    Pull
  };

  RsyncJob(Type type, const FilePath& source, const FilePath& destination);

  // Methods used in the pull configuration.
  bool sendSegments(Queue<RsyncPacket*>& rOutQueue);
  bool reassemble(Queue<RsyncPacket*>& rInQueue);

  // Methods used in the push configuration.
  bool receiveSegments(Queue<RsyncPacket*>& rInQueue);
  bool createInstructions(Queue<RsyncPacket*>& rOutQueue);

private:

  Type     mJobType;
  FilePath mSource;
  FilePath mDestination;

  // In the PULL configuration the job will allocate the segmenter and the 
  // Assembler.
  RsyncSegmenter* mpSegmenter;
  RsyncAssembler* mpAssembler;

  // In the PUSH configuration the job will allocated the Authority.
  RsyncSegmentTable*  mpSegmentTable;
  RsyncFileAuthority* mpAuthority;
};

#endif // RSYNC_JOB_H

