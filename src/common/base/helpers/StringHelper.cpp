#include "StringHelper.h"
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

//------------------------------------------------------------------------------
std::vector<std::string> StringHelper::Split(const std::string &s, 
                                             char delim,
                                             std::vector<std::string> &elems)
{
   std::stringstream ss(s);
   std::string item;
   
   while(std::getline(ss, item, delim))
   {
      elems.push_back(item);
   }
   
   return elems;
}

//------------------------------------------------------------------------------
std::vector<std::string> StringHelper::Split(const std::string &s, char delim)
{
   std::vector<std::string> elems;
   return Split(s, delim, elems);
}

//------------------------------------------------------------------------------
// trim from start
std::string &StringHelper::LTrim(std::string &s)
{
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
           std::not1(std::ptr_fun<int, int>(std::isspace))));
   return s;
}

//------------------------------------------------------------------------------
// trim from end
std::string &StringHelper::RTrim(std::string &s)
{
   s.erase(std::find_if(s.rbegin(), s.rend(), 
           std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
   return s;
}

//------------------------------------------------------------------------------
// trim from both ends
std::string &StringHelper::Trim(std::string &s)
{
   return LTrim(RTrim(s));
}

//------------------------------------------------------------------------------
std::string StringHelper::ToUpper(const std::string& s)
{
  std::stringstream ss;
  std::locale loc;
  std::string str="Test String.\n";
  for (std::string::size_type i=0; i<s.length(); ++i)
    ss << std::toupper(s[i], loc);
  return ss.str();
}

