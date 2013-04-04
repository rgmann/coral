#ifndef FILE_SYSTEM_HELPER_H
#define FILE_SYSTEM_HELPER_H

#include <string>
//struct FsStat
//{
//   
//};

class FileSystemHelper
{
public:
   
   static bool FileExists(const std::string &path);
   
   static bool Move(const std::string &src,
                    const std::string &dst,
                    bool bOverwrite = false);
   
//   static bool Stat(const std::string &path,
//                    FsStat &results);
   
   static bool Copy(const std::string &src, const std::string &dst);
   
   static std::string Concat(const std::string &a, const std::string &b);
};

#endif // FILE_SYSTEM_HELPER_H