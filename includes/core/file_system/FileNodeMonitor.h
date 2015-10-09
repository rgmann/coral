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



#ifndef FILE_NODE_MON_H
#define FILE_NODE_MON_H

#include <map>
#include <utility>
#include "FileNode.h"
#include "Queue.h"

typedef std::vector<FileNode*> FileNodeRefList;
typedef std::vector<FileNode>  FileNodeList;

class FileNodeMonitor
{
public:
   
   FileNodeMonitor();
   
   ~FileNodeMonitor();
   
   void setBase(const std::string &basePath);
   
   void setBase(FileNode &baseNode);
   
   void refresh();
      
   /**
    * Finds all nodes that no longer exist.
    * Uses directory mtime to speed up the process.
    * Renamed files are currently considered removed nodes.
    */
   
   /**
    * MTIME: changes when you write to the file. 
    * It is the age of the data in the file. 
    * Whenever mtime changes, so does ctime.
    */
   
   /**
    * But ctime changes a few extra times. 
    * For example, it will change if you change 
    * the owner or the permissions on the file.
    */
   
   enum DeltaType {
      Added = 0,   // Node was added since last refresh
      Removed, // Node was removed
      Modified, // ctime changed
      Updated
   };
   struct FileSysDelta {
      DeltaType   type;
      FileNode    node;
   };
   bool getDelta(FileSysDelta &delta, int nTimeoutMs);
   
private:
   
   void sync(FileNode &base);
   
   void syncDirectory(FileNode &oldDir);
   
   bool nodeExistsForPath(const std::string &fullPath);
   
   void addNewNode(FileNode &parent, FileNode &newNode);
   
   void registerNodes(FileNode &node, bool recurse = true);
   
   void pushDelta(DeltaType type, FileNode &node);
   
private:
   
   FileNode          m_BaseNode;
   
   // List of file/dir names to ignore when traversing.
   FileNameList      m_IgnoreList;
   
   // If a path does not appear in the LUT,
   // then it is a new file.
   std::map<std::string, FileNode> m_NodeLut;
   
   Queue<FileSysDelta> m_deltaQueue;
   FileNodeList        mAddedDirList;
   FileNodeList        mRemovedDirList;
};

#endif // FILE_NODE_MON_H
