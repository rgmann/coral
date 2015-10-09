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