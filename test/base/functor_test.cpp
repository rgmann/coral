#include <stdlib.h>     /* srand, rand */
#include <iostream>
#include <vector>
#include <algorithm>

class TwoParamFunctor {
public:

  explicit TwoParamFunctor(int mode) : mMode(mode) {};

  int mMode;

  virtual bool operator() (const int& a, const int& b)
  {
    std::cout << "base" << std::endl;
    if (mMode == 0) return a > b;
    else           return a < b;
  }
};

class DTwoParamFunctor : public TwoParamFunctor {
public:

  DTwoParamFunctor(int type, int mode) : TwoParamFunctor(mode), mType(type) {};
  int mType;

  bool operator() (const int& a, const int& b)
  {
    std::cout << "der" << std::endl;
    if (mMode == 0) return a < b;
    else           return a > b;
  }

};

class SortedVector {
public:

  SortedVector() : mFunctor(TwoParamFunctor(0)) {};

  void setComparator(TwoParamFunctor& rComp)
  {
    mFunctor = rComp;
  };

  void sort()
  {
    std::sort(mVec.begin(), mVec.end(), mFunctor);
  };

  TwoParamFunctor mFunctor;
  std::vector<int> mVec;
};


void print_vector(const std::vector<int>& vec)
{
  for (int count = 1; count < vec.size(); count++)
  {
    std::cout << vec[count - 1] << ", ";
    if (count % 10 == 0) std::cout << std::endl;
  }
  std::cout << std::endl;
} 

int main (int argc, char* argv[])
{
  SortedVector lVec;
  DTwoParamFunctor lfunc(8, 1);

  srand (time(NULL));
  for (int count = 0; count < 100; count++) lVec.mVec.push_back(rand() % 100);
  print_vector(lVec.mVec);
  std::cout << std::endl;

  lVec.sort();
  print_vector(lVec.mVec);
  std::cout << std::endl;

  lVec.setComparator(lfunc);
  lVec.sort();
  print_vector(lVec.mVec);
  std::cout << std::endl;


  return 0;
}

