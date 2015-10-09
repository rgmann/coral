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



#include "FilePath.h"

#if defined(_WIN32) || defined(_WIN64)
#define FP_SEPERATOR (const char*)"\\"
#else
#define FP_SEPERATOR (char)'/'
#endif

//------------------------------------------------------------------------------
FilePath::FilePath(const std::string &path)
: mbBeginsWithSep(false)
{
   *this = path;
}

//------------------------------------------------------------------------------
FilePath::FilePath(const FilePath &path)
: mbBeginsWithSep(false)
{
   *this = path;
}

//------------------------------------------------------------------------------
FilePath::FilePath(const std::string &path, const std::string &file)
: mbBeginsWithSep(false)
{
   *this = path;
   *this += file;
}

//------------------------------------------------------------------------------
std::string FilePath::getFullPath() const
{
   std::string lFullPath("");
   std::vector<std::string>::const_iterator liSegment;
   
   if (mbBeginsWithSep)
   {
      lFullPath += FP_SEPERATOR;
   }
   
   liSegment = mSegments.begin();
   for (; liSegment != mSegments.end(); liSegment++)
   {
      lFullPath += *liSegment;
      lFullPath += FP_SEPERATOR;
   }
   
   return lFullPath;
}

//------------------------------------------------------------------------------
std::string FilePath::getLastSegment() const
{
   std::string lLastSegment("");
   
   if (mSegments.size() > 0)
   {
      lLastSegment = mSegments.back();
   }
   
   return lLastSegment;
}

//------------------------------------------------------------------------------
std::string FilePath::getExtension() const
{
   std::string lExtension("");
   std::string lLastSegment = getLastSegment();
   
   if (!lLastSegment.empty())
   {
      size_t lnDotPos = lLastSegment.find_last_of('.');
      
      if (lnDotPos != std::string::npos)
      {
         lExtension = lLastSegment.substr(lnDotPos);
      }
   }
   
   return lExtension;
}

//------------------------------------------------------------------------------
FilePath FilePath::getSubPath(size_t first, size_t last) const
{
   FilePath lFilePath;
   lFilePath.mbBeginsWithSep = mbBeginsWithSep;
   
   if (mSegments.size() != 0)
   {
      size_t lnSegPos = first;
      
      while ((lnSegPos < mSegments.size()) && (lnSegPos <= last))
      {
         lFilePath += mSegments[lnSegPos];
      }      
   }
   
   return lFilePath;
}

//------------------------------------------------------------------------------
bool FilePath::contains(const FilePath &other, PosPair &pos)
{
   return contains(other.mSegments, pos);
}

//------------------------------------------------------------------------------
bool FilePath::contains(const std::string &path, PosPair &pos)
{
   std::vector<std::string> lvSegments;
   
   parse(path, lvSegments);
   
   return contains(lvSegments, pos);
}

//------------------------------------------------------------------------------
bool FilePath::contains(const std::vector<std::string> &segments, PosPair &pos)
{
   bool lbContains = false;
      
   if (segments.size() == 0 || segments.size() > mSegments.size())
   {
      return false;
   }
   
   std::vector<std::string>::iterator liLeftSeg = mSegments.end();
   std::vector<std::string>::const_iterator liRightSeg = segments.end();
   
   std::vector<std::string>::iterator liStart = mSegments.begin();
   std::vector<std::string>::iterator liStop = mSegments.end();
   
   for (; liLeftSeg != mSegments.begin() && !lbContains; liLeftSeg--)
   {
      if (liLeftSeg->compare(*liRightSeg) == 0)
      {         
         if (liRightSeg == segments.end())
         {
            liStop = liLeftSeg;
         }
         
         if (--liRightSeg == segments.begin())
         {
            liStart = liLeftSeg;
            lbContains = true;
         }
      }
      else
      {
         liRightSeg = segments.end();
      }
   }
   
   if (lbContains)
   {
      pos = std::make_pair(liStart, liStop);
   }
   
   return lbContains;
}

//------------------------------------------------------------------------------
FilePath& FilePath::operator= (const FilePath& other)
{
   if (this == &other)
   {
      return *this;
   }
   
   mbBeginsWithSep = other.mbBeginsWithSep;
   mSegments = other.mSegments;
   
   return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::operator= (const std::string& path)
{
   parse(path, mSegments);
   
   if (path.length() > 0)
   {
      mbBeginsWithSep = (path.find(FP_SEPERATOR) == 0);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
bool FilePath::operator== (const FilePath &other)
{
   bool lbEqual = false;
   
   if (this == &other)
   {
      return true;
   }
   
   lbEqual = (mbBeginsWithSep == other.mbBeginsWithSep);
   lbEqual &= (mSegments.size() == other.mSegments.size());
   
   if (lbEqual)
   {
      std::vector<std::string>::const_iterator liLeftSeg;
      std::vector<std::string>::const_iterator liRightSeg;
      
      liLeftSeg = mSegments.begin();
      liRightSeg = other.mSegments.begin();
      
      while (true)
      {
         if (liLeftSeg == mSegments.end() ||
             liRightSeg == mSegments.end())
         {
            break;
         }
         
         lbEqual &= (liLeftSeg->compare(*liRightSeg) == 0);
         
         liLeftSeg++;
         liRightSeg++;
      }
   }
   
   return lbEqual;
}

//------------------------------------------------------------------------------
bool FilePath::operator== (const std::string &path)
{
   FilePath lFilePath(path);
   
   return (*this == lFilePath);
}

//------------------------------------------------------------------------------
FilePath& FilePath::operator+= (const FilePath &other)
{
   std::vector<std::string> lvSegments = other.mSegments;
   
   std::vector<std::string>::const_iterator liSegment = other.mSegments.begin();
   for (; liSegment != lvSegments.end(); liSegment++)
   {
      mSegments.push_back(*liSegment);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::operator+= (const std::string &path)
{
   std::vector<std::string> lvSegments;
   
   parse(path, lvSegments);
   
   std::vector<std::string>::const_iterator liSegment = lvSegments.begin();

   for (; liSegment != lvSegments.end(); liSegment++)
   {
      mSegments.push_back(*liSegment);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::operator-= (const FilePath &other)
{
   PosPair lPos;
   
   // Not very efficient, but make sure that all segments from other exist in
   // this FilePath and that they are contiguous.
   if (!contains(other, lPos))
   {
      return *this;
   }
   
   mSegments.erase(lPos.first, lPos.second);
   
   return *this;
}

//------------------------------------------------------------------------------
FilePath& FilePath::operator-= (const std::string &path)
{
   PosPair lPos;
   std::vector<std::string> lvSegments;
   
   parse(path, lvSegments);
   
   // Not very efficient, but make sure that all segments from other exist in
   // this FilePath and that they are contiguous.
   if (!contains(path, lPos))
   {
      return *this;
   }
   
   mSegments.erase(lPos.first, lPos.second);
   
   return *this;
}

//------------------------------------------------------------------------------
FilePath operator+ (const FilePath& left, const FilePath& right)
{
   FilePath lFilePath;
   
   lFilePath += left;
   lFilePath += right;
   
   return lFilePath;
}

//------------------------------------------------------------------------------
FilePath operator+ (const FilePath& left, const std::string& right)
{
   FilePath lFilePath;
   
   lFilePath += left;
   lFilePath += right;
   
   return lFilePath;
}

//------------------------------------------------------------------------------
FilePath operator+ (const std::string& left, const FilePath& right)
{
   FilePath lFilePath;
   
   lFilePath += left;
   lFilePath += right;
   
   return lFilePath;
}

//------------------------------------------------------------------------------
FilePath operator- (const FilePath& left, const FilePath& right)
{
   FilePath lFilePath(left);
   
   lFilePath -= right;
   
   return lFilePath;
}

//------------------------------------------------------------------------------
FilePath operator- (const FilePath& left, const std::string& right)
{
   FilePath lFilePath(left);
   
   lFilePath += right;
   
   return lFilePath;
}

//------------------------------------------------------------------------------
FilePath operator- (const std::string& left, const FilePath& right)
{
   FilePath lFilePath(left);
   
   lFilePath -= right;
   
   return lFilePath;
}

//------------------------------------------------------------------------------
void FilePath::parse(const std::string          &path, 
                     std::vector<std::string>   &segments)
{   
   std::string lPathStr(path);
   std::string lSegment;
   
   size_t lnSepPos = 0;
   size_t lnPos = 0;
   size_t lnLength = 0;
   
   segments.clear();
   
   while (true)
   {
      lSegment.clear();
      
      lnSepPos = lPathStr.find(FP_SEPERATOR, lnPos);
      
      lnLength = std::string::npos;
      if (lnSepPos != std::string::npos)
      {
         lnLength = lnSepPos - lnPos;
      }
      
      lSegment = lPathStr.substr(lnPos, lnLength);
      
      if (!lSegment.empty())
      {
         mSegments.push_back(lSegment);
      }
      
      if (lnSepPos == std::string::npos)
      {
         break;
      }
      
      lnPos = lnSepPos + 1;
   }
}
