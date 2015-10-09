// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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
  for (std::string::size_type i=0; i<s.length(); ++i)
    ss << std::toupper(s[i], loc);
  return ss.str();
}

//------------------------------------------------------------------------------
std::string StringHelper::ToLower(const std::string& s)
{
  std::stringstream ss;
  std::locale loc;
  for (std::string::size_type i=0; i<s.length(); ++i)
    ss << std::tolower(s[i], loc);
  return ss.str();
}

