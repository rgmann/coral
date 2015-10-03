#include <stdio.h>
#include <string.h>
#include "HashTable.h"

class Thingy {
public:

  enum { DataSize = 10 };

  Thingy(int id) : mID(id)
  {
  }

  int id() const { return mID; };

  char* const data() { return mData; };
  const char* const data() const { return mData; };

  void print() const
  {
    printf("%2d | ", id());
    for (int lnByte = 0; lnByte < DataSize; lnByte++)
      printf("%02X ", mData[lnByte]);
    printf("\n");
  }

private:

  char mData[DataSize];
  int mID;
};

class ThingyComparator {
public:

  ThingyComparator(const Thingy* pThingy) : mpThingy(pThingy) {}

  bool operator() (Thingy* pThingy)
  {
    for (int nByte = 0; nByte < Thingy::DataSize; nByte++)
    {
      if (mpThingy->data()[nByte] != pThingy->data()[nByte]) return false;
    }
    return true;
  }

private:

  const Thingy* mpThingy;
};


class ThingyDestroyer {
public:

  void operator()(Thingy*& pThingy)
  {
    if (pThingy)
    {
      delete pThingy;
      pThingy = NULL;
    }
  }
};


int main()
{
  Thingy* thing1 = new Thingy(1);
  Thingy* thing2 = new Thingy(2);
  Thingy* thing3 = new Thingy(4);
  Thingy* thing4 = new Thingy(5);
  Thingy* thing5 = new Thingy(21);
  Thingy* thing6 = new Thingy(37);

  strncpy(thing1->data(), "The", Thingy::DataSize);
  strncpy(thing2->data(), "quick", Thingy::DataSize);
  strncpy(thing3->data(), "brown", Thingy::DataSize);
  strncpy(thing4->data(), "fox", Thingy::DataSize);
  strncpy(thing5->data(), "jumped", Thingy::DataSize);
  strncpy(thing6->data(), "over", Thingy::DataSize);

  liber::HashTable<16, Thingy*> table;

  table.insert(thing1->id(), thing1);
  table.insert(thing2->id(), thing2);
  table.insert(thing3->id(), thing3);
  table.insert(thing4->id(), thing4);
  table.insert(thing5->id(), thing5);
  table.insert(thing6->id(), thing6);

  ThingyComparator comp(thing4);
  Thingy* pThing = NULL;
  if (table.remove(thing4->id(), pThing, comp))
  {
    printf("Removed:");
    pThing->print();
  }
  else
  {
    printf("Failed to remove item\n");
  }

  ThingyComparator comp2(thing5);
  pThing = NULL;
  if (table.find(thing5->id(), pThing, comp2))
  {
    printf("Found:");
    pThing->print();
  }
  else
  {
    printf("Failed to find item\n");
  }


  ThingyDestroyer destroyer;
  table.clear(destroyer);

  if (thing4)
  {
    delete thing4;
    thing4 = NULL;
  }

  return 0;
}

