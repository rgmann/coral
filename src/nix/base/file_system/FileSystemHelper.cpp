#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "FileSystemHelper.h"

//------------------------------------------------------------------------------
bool FileSystemHelper::FileExists(const std::string &path, bool bAllowDir )
{
   bool bExists = true;
   struct stat statResults;

   bExists &= ( stat( path.c_str(), &statResults ) == 0 );

   if ( bExists && !bAllowDir )
   {
      bExists &= S_ISDIR( statResults.st_mode );
   }
   
   return bExists;
}

//------------------------------------------------------------------------------
bool FileSystemHelper::Move(const std::string &src,
                            const std::string &dst,
                            bool bOverwrite)
{
   // If the source file doesn't exist, we can't rename it.
   if (!FileExists(src))
   {
      return false;
   }
   
   // If the destination file already exists and we cannot overwrite it,
   // then we cannot do anything.
   if (FileExists(dst) && !bOverwrite)
   {
      return false;
   }
   
   return (rename(src.c_str(), dst.c_str()) == 0);
}

//------------------------------------------------------------------------------
bool FileSystemHelper::Copy(const std::string &src, const std::string &dst)
{
   char buf[BUFSIZ];
   size_t size;
   
   int l_fdSource = -1;
   int l_fdDest = -1;
   
   l_fdSource = open(src.c_str(), O_RDONLY, 0);
   if (l_fdSource == -1)
   {
      return false;
   }
   
   l_fdDest = open(dst.c_str(), O_WRONLY | O_CREAT, 0644);
   if (l_fdDest == -1)
   {
      close(l_fdDest);
      return false;
   }
   
   while ((size = read(l_fdSource, buf, BUFSIZ)) > 0)
   {
      write(l_fdDest, buf, size);
   }
   
   close(l_fdSource);
   close(l_fdDest);
   
   return false;
}

//------------------------------------------------------------------------------
std::string FileSystemHelper::Concat(const std::string &a, const std::string &b)
{
   std::string l_sAmod = a;
   std::string l_sBmod = b;
   
//   if (a.length() == 0 || (a.length() > 0 && a[a.length() - 1] != '/'))
   if (a.length() > 0 && a[a.length() - 1] != '/')
   {
      l_sAmod.push_back('/');
   }
   
   if (b.length() > 0 && b[0] == '/')
   {
      l_sBmod = b.substr(1);
   }
   
   return l_sAmod + l_sBmod;
}
