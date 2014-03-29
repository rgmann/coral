namespace liber {
namespace rsync {

class SegmentVector : public liber::rsync::SegmentAccessor {
public:

  SegmentVector();
  virtual ~SegmentVector();

  virtual Segment* getSegment(SegmentID id);

private:

  std::vector<Segment*> mSegments;

};

} // End namespace rsync
} // End namespace liber

