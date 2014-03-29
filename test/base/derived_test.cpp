#include <string>
#include <string.h>
#include <stdio.h>


class Base {
public:

  enum Type {
    TypeSegment,
    TypeChunk,
  };

  Base(Type type) { mType = type; };
  virtual ~Base() {};

  Type getType() const { return mType; };

  virtual std::string toString() const = 0;

private:

  Type mType;
};

class Segment : public Base {
public:

  Segment(int id) : Base(TypeSegment), mId(id) {};

  int getId() const { return mId; };

  std::string toString() const { return "Segment"; };

private:

  int mId;
};

class Chunk : public Base {
public:

  Chunk(char* data, int size) : Base(TypeChunk), mpData(NULL), mSize(size)
  {
    mpData = new char[mSize];
    memcpy(mpData, data, size);
  };
  ~Chunk()
  {
    if (mpData) delete mpData;
  };

  char* const data() const { return mpData; };
  int size() const { return mSize; };
  std::string toString() const { return "Chunk"; };

private:

  char* mpData;
  int mSize;
};

void execute(Base* pBase)
{
  printf("Execute base\n");
}
void execute(Segment* pSegment)
{
  printf("Execute segment\n");
}
void execute(Chunk* pChunk)
{
  printf("Execute chunk\n");
}

int main (int argc, char* argv[])
{
  char* lstr = "test string";
  Segment lSegment(15);
  Chunk   lChunk(lstr, strlen(lstr) + 1);

  execute(&lSegment);
  execute(&lChunk);

  Base* pBase = new Segment(13);
  execute(pBase);
  delete pBase;

  return 0;
}

