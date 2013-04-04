#include "RsyncSegmenter.h"
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

int main(int argc, char *argv[])
{
   RsyncSegmenter segmenter;
      
   printf("Segmenter test\n");
   
   segmenter.setSegmentSize(6);
   segmenter.process("testfiles/rsync_fileA.dat");
   
   return 0;
}

