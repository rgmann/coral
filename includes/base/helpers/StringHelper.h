#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <string>
#include <vector>

class StringHelper
{
public:
   
   static std::vector<std::string> Split(const std::string &s, 
                                         char delim,
                                         std::vector<std::string> &elems);
   
   static std::vector<std::string> Split(const std::string &s, char delim);
   
   static std::string& LTrim(std::string &s);
   
   static std::string& RTrim(std::string &s);
   
   static std::string& Trim(std::string &s);
   
private:
   
   StringHelper();
   StringHelper(const StringHelper& other);
};

#endif // STRING_HELPER_H