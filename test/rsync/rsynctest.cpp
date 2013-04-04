#include "ArgParser.h"
#include "RsyncSegment.h"
#include "SimpleHashTable.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#define  SEG_LEN  6

void PrintWeak(const Adler32Checksum &sum);
void PrintStrong(const Hash128 &sum);
void PrintBoth(const Adler32Checksum &sum, const Hash128 &sum);

void SegmentFile(const std::string &filename, 
                 std::vector<RsyncSegment*> &segments, 
                 int stride, bool bRoll);

void Reconstruct(const std::string &filename,
                 std::vector<RsyncSegment*> &client,
                 SimpleHashTable<16, RsyncSegment*>  &clientSegHash);

bool ConfirmMatch(RsyncSegment* pSegA, RsyncSegment* pSegB);

//void PrintSeg(unsigned char* pBlock, int nBytes)
//{
//   for (int ind = 0; ind < nBytes; ind++)
//      printf("%d ", *(pBlock + ind));
//   printf("\n");
//}

int main(int argc, char *argv[])
{
   ArgParser args;
   
   std::ifstream serverFile;
   std::vector<RsyncSegment*> serverSegments;
   
   std::ifstream clientFile;
   std::vector<RsyncSegment*> clientSegments;
   std::vector<RsyncSegment*>::iterator it;
    
   int l_nSegNumber = 0;
   
   //char clientBuffer[256];
   //int l_nBytes = 0;
   //int l_nSegBytes = 0;
   Adler32Checksum   weaksum;
   Md5Hash           strongsum;
   Hash128           stronghash;
   
   SimpleHashTable<16, RsyncSegment*>   serverSegHash;
   
   std::string fileA = "testfiles/rsync_fileA.dat";
   std::string fileB = "testfiles/rsync_fileB.dat";
   
   printf("Rsync test\r\n");
   
   // Segment the client file into non-overlapping segments of length SEG_LEN
   SegmentFile(fileB, clientSegments, SEG_LEN, false);
   
   // Segment the server file int into overlapping segments of length SEG_LEN.
   // Enable rolling checksum.
   SegmentFile(fileA, serverSegments, 1, true);
   
   // Iterate through list of client file segments and add them to a hash table.
   it = clientSegments.begin();
   for (; it < clientSegments.end(); ++it)
   {
      (*it)->getWeak(weaksum);
      (*it)->getStrong(strongsum);
      serverSegHash.add(weaksum.s, *it);
      
      strongsum.get(&stronghash);
      
      printf("Client #%d:", l_nSegNumber++);
      PrintBoth(weaksum,stronghash);
      printf("\n");
   }
   std::cout << std::endl;
   
   
//   bool l_bFound = false;
//   //RsyncSegment*  pTempSeg = NULL;
//   l_nSegNumber = 0;
//   it = serverSegments.begin();
//   for (; it < serverSegments.end(); ++it)
//   {
//      (*it)->getWeak(weaksum);
//      //(*it)->getStrong(strongsum);
//      
//      //strongsum.get(&stronghash);
//      
//      serverSegHash.get(weaksum.s, l_bFound);
//      if (l_bFound)
//      {
//         printf("Server match at #%d: ", l_nSegNumber++);
//         PrintBoth(weaksum,stronghash);
//         printf("\n");
//      }
//   }
   Reconstruct("testfiles/reconstructed.dat",
               serverSegments, serverSegHash);
   
   //clientFile.close();
   
   return 0;
}

void PrintWeak(const Adler32Checksum &sum)
{
   std::cout << std::hex << sum.s;
}

void PrintStrong(const Hash128 &sum)
{
   for (int l_nIdx = 0; l_nIdx < 16; l_nIdx++)
      printf("%02x", sum.b[l_nIdx]);
      //std::cout << std::hex << sum.b[l_nIdx];

}

void PrintBoth(const Adler32Checksum &weak, const Hash128 &strong)
{
   PrintWeak(weak);
   printf(", ");
   PrintStrong(strong);
}

void SegmentFile(const std::string &filename, 
                 std::vector<RsyncSegment*> &segments, 
                 int stride, bool bRoll)
{
   int l_nBytes = 0;
   int l_nSegNumber = 0;
   char buffer[256];
   
   Adler32Checksum   oldweaksum;
   
   std::ifstream is;
   is.open(filename.c_str(), std::ios::binary);
   
   std::cout << filename << ": ";
   while (is.good()) {
      buffer[l_nBytes] = is.get();       // get character from file
      if (is.good())
         std::cout << std::hex << buffer[l_nBytes];
      
      l_nBytes++;
   }
   std::cout << std::endl;
   
   for (int l_nOffs = 0; l_nOffs < l_nBytes; l_nOffs++)
   {
      if ((l_nOffs % stride) == 0)
      {
         Adler32Checksum* pAdler = (bRoll && (l_nOffs > 0)) ? &oldweaksum : NULL;
         RsyncSegment* l_pSeg = NULL;
         int l_nSegLen = ((l_nBytes - l_nOffs) < SEG_LEN) ? (l_nBytes - l_nOffs) : SEG_LEN;
         
         l_pSeg = RsyncSegment::Create((unsigned char*)buffer,
                                       l_nOffs,
                                       l_nSegLen,
                                       pAdler);
         segments.push_back(l_pSeg);
         l_pSeg->getWeak(oldweaksum);
      }
   }
   
   is.close();
}

void Reconstruct(const std::string &filename,
                 std::vector<RsyncSegment*> &server,
                 SimpleHashTable<16, RsyncSegment*>  &clientSegHash)
{
   std::ofstream os;
   
   Adler32Checksum   weaksum;
   Md5Hash           strongsum;
   Hash128           stronghash;
   bool l_bFound = false;
   
   RsyncSegment*     clientSeg = NULL;
   std::vector<RsyncSegment*>::iterator it;
   
   int l_nSegNumber = 0;
   
   os.open(filename.c_str(), std::ios::binary);
   if (os.fail())
   {
      printf("Reconstruct: failed to open file\n");
      return;
   }
   
   it = server.begin();
   for (; it < server.end(); ++it)
   {
      (*it)->getWeak(weaksum);
      //(*it)->getStrong(strongsum);
      
      //strongsum.get(&stronghash);
      
      clientSeg = clientSegHash.get(weaksum.s, l_bFound);
      if (l_bFound && ConfirmMatch(*it, clientSeg))
      {
         printf("Server match at #%d: ", l_nSegNumber++);
         PrintBoth(weaksum, stronghash);
         printf("\n");
         
         os.write((char*)clientSeg->getBlockPtr(), clientSeg->getBlockSize());
         
         for (unsigned int l_nIdx = 0;
              l_nIdx < clientSeg->getBlockSize() - 1; l_nIdx++)
         {
            it++;
         }
      }
      else
      {
         char* l_pBlock = (char*)(*it)->getBlockPtr();
         os.put(l_pBlock[0]);
      }
   }
   
   os.close();
}

bool ConfirmMatch(RsyncSegment* pSegA, RsyncSegment* pSegB)
{
   Md5Hash           ssA, ssB;
   Hash128           shA, shB;
   
   if (pSegA == NULL || pSegB == NULL) return false;
   
   pSegA->getStrong(ssA);
   ssA.get(&shA);
   
   pSegB->getStrong(ssB);
   ssB.get(&shB);
   
   for (int l_nIdx = 0; l_nIdx < 16; l_nIdx++)
   {
      if (shA.b[l_nIdx] != shB.b[l_nIdx]) return false;
   }
   
   return true;
}
