#include <string.h>
#include <stdio.h>
#include <sstream>
#include "Segment.h"
#include "PacketHelper.h"
#include "CircularBuffer.h"

using namespace liber::rsync;
using namespace liber::netapp;

struct PackedSegment {
  Segment::ID id;
  ui32        size;
  ui32        offset;
  i32         weak;
  Hash128     strong;
};


static const ui32 M = 0x00010000;

void PrintSegRsync(unsigned char* pBlock, int nBytes)
{
   for (int ind = 0; ind < nBytes; ind++)
      printf("%d ", *(pBlock + ind));
   printf("\n");
}

//------------------------------------------------------------------------------
Segment::Segment()
: mID(-1)
, mpData(NULL)
, mnSegmentSize(0)
, mnOffset(0)
, mnVSegmentSize(0)
{
}

//------------------------------------------------------------------------------
Segment::Segment(ID id, ui32 offset)
: mID(id)
, mpData(NULL)
, mnSegmentSize(0)
, mnOffset(offset)
, mnVSegmentSize(0)
{
}

//------------------------------------------------------------------------------
Segment::Segment(ui32 nVSegmentSize)
: mID(-1)
, mpData(NULL)
, mnSegmentSize(0)
, mnOffset(0)
, mnVSegmentSize(nVSegmentSize)
{
  mpData = new ui8[mnVSegmentSize];
}

//------------------------------------------------------------------------------
Segment::Segment(const Segment& other)
{
  mID = other.mID;
  mnSegmentSize = other.mnSegmentSize;
  mnVSegmentSize = other.mnVSegmentSize;
  mnOffset = other.mnOffset;
  mWeak = other.mWeak;
  if (other.mpData)
  {
    mpData = new ui8[mnVSegmentSize];
    memcpy(mpData, other.mpData, mnVSegmentSize);
  }
}

//------------------------------------------------------------------------------
Segment::~Segment()
{
  if (mpData)
  {
    delete[] mpData;
    mpData = NULL;
  }
}

//------------------------------------------------------------------------------
void Segment::
setData(const ui8* pData, ui32 nVSegmentSize, ui32 nSegmentSize, const Adler32Checksum* pPrev)
{
  mStrong.invalidate();

  mnSegmentSize = nSegmentSize;

  if (mpData && (mnVSegmentSize != nVSegmentSize))
  {
    delete[] mpData;
    mpData = NULL;
  }
  if (mpData == NULL)
  {
    //mpData = new ui8[mnSegmentSize];
    mpData = new ui8[nVSegmentSize];
  }
  mnVSegmentSize = nVSegmentSize;

  memcpy(mpData, pData, mnSegmentSize);

  if (pPrev)
  {
    rollWeak(mWeak, mnOffset, nVSegmentSize, *pPrev);
  }
  else
  {
    computeWeak(mWeak, mnOffset, nVSegmentSize);
  }
}

//------------------------------------------------------------------------------
void Segment::
setData(CircularBuffer& buffer, ui32 nVSegmentSize, const Adler32Checksum* pPrev)
{
  mStrong.invalidate();

  //mnSegmentSize = nSegmentSize;

  if (mpData && (mnVSegmentSize != nVSegmentSize))
  {
    delete[] mpData;
    mpData = NULL;
  }
  if (mpData == NULL)
  {
    //mpData = new ui8[mnSegmentSize];
    mpData = new ui8[nVSegmentSize];
  }
  mnVSegmentSize = nVSegmentSize;

  //if (nSegmentSize < nVSegmentSize) memset(mpData, 0, mnVSegmentSize);
  //memcpy(mpData, pData, mnSegmentSize);
  mnSegmentSize = buffer.peek((char*)mpData, mnVSegmentSize);

  if (pPrev)
  {
    rollWeak(mWeak, mnOffset, nVSegmentSize, *pPrev);
  }
  else
  {
    computeWeak(mWeak, mnOffset, nVSegmentSize);
  }
}

//------------------------------------------------------------------------------
void Segment::
setData(std::istream& stream, ui32 nVSegmentSize, const Adler32Checksum* pPrev)
{
  mStrong.invalidate();

  if (mpData && (mnVSegmentSize != nVSegmentSize))
  {
    delete[] mpData;
    mpData = NULL;
  }
  if (mpData == NULL)
  {
    mpData = new ui8[nVSegmentSize];
  }
  mnVSegmentSize = nVSegmentSize;

  stream.read((char*)mpData, mnVSegmentSize);
  mnSegmentSize = stream.gcount();

  if (pPrev)
  {
    rollWeak(mWeak, mnOffset, nVSegmentSize, *pPrev);
  }
  else
  {
    computeWeak(mWeak, mnOffset, nVSegmentSize);
  }
}

//------------------------------------------------------------------------------
const Adler32Checksum& Segment::getWeak()
{
  return mWeak;
}

//------------------------------------------------------------------------------
Md5Hash& Segment::getStrong()
{
  // If the strong checksum has not been computed, and the data is set,
  // compute it now.
  if (mpData && !mStrong.isValid())
  {
    mStrong.hashify(mpData, mnSegmentSize);
  }

  return mStrong;
}

//------------------------------------------------------------------------------
Md5Hash& Segment::getStrong(std::ifstream& istream)
{
  Segment::data(istream);
  return Segment::getStrong();
}

//------------------------------------------------------------------------------
ui8* const Segment::data()
{
   return mpData;
}

//------------------------------------------------------------------------------
ui8* const Segment::data(std::ifstream &istream)
{
  if (mpData)
  {
    delete[] mpData;
    mpData = NULL;
  }
  mpData = new ui8[mnSegmentSize];

  istream.seekg(mnOffset, istream.beg);
  istream.read((char*)mpData, mnSegmentSize);

  return Segment::data();
}

//------------------------------------------------------------------------------
ui32 Segment::size() const
{
  return mnSegmentSize;
}

//------------------------------------------------------------------------------
Segment::ID Segment::getID() const
{
  return mID;
}

//------------------------------------------------------------------------------
bool Segment::endOfStream() const
{
  return (getID() == -1);
}

//------------------------------------------------------------------------------
void Segment::release()
{
  mpData = NULL;
  mnSegmentSize = 0;
}

//------------------------------------------------------------------------------
ui8 Segment::getByte(ui32 nOffset, OffsetBase base) const
{
  ui8 lnByte = 0;

  if (base == StreamStart)
  {
    nOffset = (nOffset > mnOffset) ? (nOffset - mnOffset) : 0;
  }

  if (mpData && nOffset < size())
  {
    lnByte = mpData[nOffset];
  }

  return lnByte;
}

//------------------------------------------------------------------------------
bool Segment::isValid() const
{
  bool lbValid = true;
  lbValid &= (mID >= 0);
  lbValid &= (mnSegmentSize > 0);
  return lbValid;
}

//------------------------------------------------------------------------------
/*bool Segment::pack(ui8** ppData, ui32& nSizeBytes)
{
  bool lbValid = false;

  if ((lbValid = isValid()))
  {
    PackedSegment* lpSegment = reinterpret_cast<PackedSegment*>(*ppData);
    lpSegment = new PackedSegment;

    lpSegment->id     = mID;
    lpSegment->size   = size();
    lpSegment->offset = mnOffset;
    lpSegment->weak   = mWeak.checksum();
    getStrong().get(&lpSegment->strong);
  }

  return lbValid;
}*/

//------------------------------------------------------------------------------
std::string Segment::serialize()
{
  PacketCtor ctor;

  ctor.write((ui32)mID);
  ctor.write(size());
  ctor.write(mnOffset);
  ctor.write((ui32)mWeak.checksum());

  std::string strongBuffer;
  Hash128 strong;
  getStrong().get(&strong);
  strongBuffer.assign((char*)&strong, sizeof(strong));
  ctor.write(strongBuffer);

  return ctor.stream.str(); 
}

//------------------------------------------------------------------------------
bool Segment::deserialize(const char* pData, ui32 nSizeBytes)
{
  PacketDtor dtor;
  dtor.stream.write(pData, nSizeBytes);
  return deserialize(dtor);
}

//------------------------------------------------------------------------------
bool Segment::deserialize(const std::string& data)
{
  PacketDtor dtor;
  dtor.setData(data);
  return deserialize(dtor);
}

//------------------------------------------------------------------------------
bool Segment::deserialize(PacketDtor& dtor)
{
  bool lbSuccess = true;

  lbSuccess &= dtor.read(mID);
  lbSuccess &= dtor.read(mnSegmentSize);
  lbSuccess &= dtor.read(mnOffset);
  lbSuccess &= dtor.read(mWeak.s);

  std::string strongBuffer;
  lbSuccess &= dtor.read(strongBuffer);
  if (lbSuccess)
  {
    Hash128 strong;
    memcpy(&strong, strongBuffer.data(), strongBuffer.size());
    mStrong = Md5Hash(strong);
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
std::string Segment::debugDump()
{
  std::stringstream ss;

  ss << "Segment #" << getID()
     << " (size=" << size() 
     << ", weak=" << std::hex << getWeak().checksum() 
     << ")" << std::endl;
  if (data())
  {
    ui8* lpData = data();
    for (ui32 lnOffset = 0; lnOffset < size(); lnOffset++)
    {
      ss << " 0x" << std::hex << (ui16)lpData[lnOffset];
      if ((lnOffset > 0) && (lnOffset % 16 == 0))
      {
        ss << std::endl;
      }
    }
  }
  else
  {
    ss << "  Empty" << std::endl;
  }

  return ss.str();
}

//------------------------------------------------------------------------------
void Segment::
computeWeak(Adler32Checksum& next,
            ui32             nOffset,
            ui32             nBlockSizeBytes)
{
#ifdef DEBUG_WEAK_CSUM
  printf("ComputeWeak: offs=%u, size=%u\n", nOffset, nBlockSizeBytes);
#endif

  next.k = nOffset;
  next.l = (nOffset + nBlockSizeBytes - 1);

  next.a = 0;
  for (ui32 lnByte = 0; lnByte < nBlockSizeBytes; ++lnByte)
  {
    next.a += (int)getByte(lnByte);
  }
   
  next.b = 0;
  for (ui32 lnByte = 0; lnByte < nBlockSizeBytes; ++lnByte)
  {
    next.b += (next.l - (next.k + lnByte) + 1) * (int)getByte(lnByte);
  }


  next.s = ((next.b << 16) & 0xFFFF0000) | (next.a & 0x0000FFFF);
  next.x_k = getByte(next.k - nOffset);
  next.x_l = getByte(next.l - nOffset);


#ifdef DEBUG_WEAK_CSUM
  printf("CW(%u): k = %d, l = %d, a = %d, b = %d, s = %x, (%c,%c)\n", 
         nOffset, next.k, next.l, next.a, next.b, next.s, next.x_k, next.x_l);
#endif
}

//------------------------------------------------------------------------------
void Segment::
rollWeak(Adler32Checksum&       next,
         ui32                   nOffset,
         ui32                   nBlockSizeBytes,
         const Adler32Checksum& prev)
{
  next.k = prev.k + 1;
  next.l = prev.l + 1;

  // Transform from file offset to offset within the segment. 
  next.a = (prev.a - prev.x_k);
  next.a += (int)getByte(next.l - nOffset);

  next.b = (prev.b - ((prev.l - prev.k + 1) * prev.x_k) + next.a);

  next.s = ((next.b << 16) & 0xFFFF0000) | (next.a & 0x0000FFFF);
  next.x_k = getByte(next.k - nOffset);
  next.x_l = getByte(next.l - nOffset);


#ifdef DEBUG_WEAK_CSUM
  printf("RW(%u): k = %d, l = %d, a = %d, b = %d, s = %x, (l-k+1)*x_k=%u, (0x%02X,0x%02X)\n", 
         next.l - nOffset, next.k, next.l, next.a, next.b, next.s, (prev.l-prev.k+1)*prev.x_k, next.x_k, next.x_l);
#endif
}



//------------------------------------------------------------------------------
SegmentComparator::SegmentComparator(Segment* pSegment)
: mpSegment(pSegment)
{
}

//------------------------------------------------------------------------------
bool SegmentComparator::operator() (Segment* pSegment)
{
  bool lbEqual = false;

  if (mpSegment->getWeak().checksum() == pSegment->getWeak().checksum())
  {
    if (mpSegment->getStrong() == pSegment->getStrong())
    {
      lbEqual = true;
    }
  }

  return lbEqual;
}



//------------------------------------------------------------------------------
void SegmentDestructor::operator() (Segment*& pSegment)
{
  if (pSegment)
  {
    delete pSegment;
    pSegment = NULL;
  }
}


