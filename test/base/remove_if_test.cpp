#include <iostream>     // std::cout
#include <algorithm>    // std::remove_if
#include <vector>

bool IsOdd (int i) { return ((i%2)==1); }

int main () {
  int myints[] = {1,2,3,4,5,6,7,8,9};            // 1 2 3 4 5 6 7 8 9
  std::vector<int> vints;
  for (int ind = 0; ind < sizeof(myints)/sizeof(int); ++ind) vints.push_back(myints[ind]);

  // bounds of range:
  vints.erase(std::remove_if(vints.begin(), vints.end(), IsOdd), vints.end());   // 2 4 6 8 ? ? ? ? ?
                                                 // ^       ^
  std::cout << "the range contains:";
  std::vector<int>::iterator vit = vints.begin();
  for (; vit != vints.end(); ++vit)
  {
    std::cout << " " << *vit;
  }
  std::cout << std::endl;

  return 0;
}

