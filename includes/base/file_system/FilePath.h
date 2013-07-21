#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>
#include <vector>
#include <utility>

class FilePath
{
public:
   
   FilePath(const std::string &path = "");
   
   FilePath(const FilePath &path);
   
   FilePath(const std::string &path,
            const std::string &file);
   
   std::string getFullPath() const;
   
   std::string getLastSegment() const;
   
   std::vector<std::string> getSegments() const;
   
   /**
    * Assumes last segment is a file name.
    */
   std::string getExtension() const;
   
   FilePath    getSubPath(size_t first, size_t last) const;
   
   typedef     std::pair<std::vector<std::string>::iterator,
                         std::vector<std::string>::iterator> PosPair;
   
   bool        contains(const FilePath &other, PosPair &pos);
   bool        contains(const std::string &path, PosPair &pos);
   
   FilePath&   operator= (const FilePath&);
   FilePath&   operator= (const std::string&);
   
   bool        operator== (const FilePath&);
   bool        operator== (const std::string&);
   
   FilePath&   operator+= (const FilePath&);
   FilePath&   operator+= (const std::string&);
   
   FilePath&   operator-= (const FilePath&);
   FilePath&   operator-= (const std::string&);
   
   friend FilePath operator+ (const FilePath& left, const FilePath& right);
   friend FilePath operator+ (const FilePath& left, const std::string& right);
   friend FilePath operator+ (const std::string& left, const FilePath& right);
   
   friend FilePath operator- (const FilePath& left, const FilePath& right);
   friend FilePath operator- (const FilePath& left, const std::string& right);
   friend FilePath operator- (const std::string& left, const FilePath& right);
      
private:
   
   bool contains(const std::vector<std::string> &segments, PosPair &pos);
   
   void parse(const std::string &path, std::vector<std::string> &segments);
   
private:
   
   bool mbBeginsWithSep;

   std::vector<std::string> mSegments;
};

#endif // FILE_PATH_H