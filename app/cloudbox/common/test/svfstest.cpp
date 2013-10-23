#include <string>
#include <iostream>
#include <sys/statvfs.h>

float getDiskUtilization(const char* path)
{
   float lUtil = -1.0;
   struct statvfs lsres;

   if (statvfs(path, &lsres) == 0)
   {
      printf("total = %u\n", lsres.f_blocks);
      printf("avail = %u\n", lsres.f_bfree);
      printf("free = %u\n", lsres.f_bavail);
      lUtil = (float)(lsres.f_blocks - lsres.f_bavail) / (float)lsres.f_blocks;
   }

   return lUtil;
}

void gen(unsigned int bytes)
{
   FILE* lfd = NULL;
   static unsigned int fileCount = 0;
   char buffer[256];
  
   snprintf(buffer, sizeof(buffer), "./output/f%u.bin", fileCount++);
   lfd = fopen(buffer, "w+");
   if (lfd)
   {
      for (unsigned int item = 0; item < bytes / sizeof(unsigned int); item++)
      {
         double value = (rand() / (double)RAND_MAX) * 4294967296.0;
         unsigned int i_value = (unsigned int)value;
         fwrite(&i_value, sizeof(i_value), 1, lfd);
      }
   }
   fclose(lfd);
}

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      printf("Too few arguments!\n");
      return -1;
   }

   printf("utilization = %f\n", getDiskUtilization(argv[1]));
   for (int i = 0; i < 30; i++) gen(209715200);
   printf("utilization = %f\n", getDiskUtilization(argv[1]));

   return 0;
}

