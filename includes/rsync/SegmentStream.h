namespace liber {
namespace rsync {

class SegmentStream : public liber::rsync::SegmentReceiver {
public:

  SegmentStream();
  ~SegmentStream();

  void push(Segment* pSegment);

  Segment* pop(int nTimeoutMs);

private:

  Queue<Segment*> mSegments;
};

} // End namespace rsync
} // End namespace liber

