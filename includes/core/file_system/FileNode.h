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



#ifndef FILE_NODE_H
#define FILE_NODE_H

#include <string>
#include <vector>

typedef unsigned char            perm_t;
typedef std::vector<std::string> FileNameList;

class FileNode {
public:
   
   FileNode();
   
   FileNode(const FileNode &node);
   
   ~FileNode();
   
   FileNode & operator=(const FileNode &rhs);
   
   bool operator==(const FileNode &other) const;
   
   bool operator!=(const FileNode &other) const;
   
   bool inIgnoreList(const FileNameList &names) const;
   
   bool inHideList(const FileNameList &names);
   
   /**
    * Re-stats the file and updates all fields except the child list.
    */
   bool  update();
   
   /**
    * Check if the file access time has changed.
    * 
    * @param   update   If true, the access time is update.
    * @ret     bool     Returns true if the file access time
    *                   has changed.
    */
   bool  accessed(bool update = true);
   
   /**
    * Check if the file modification time has changed.
    * 
    * @param   update   If true, the modification time is update.
    * @ret     bool     Returns true if the file modification time
    *                   has changed.
    */
   bool  modified(bool update = true);
   
   /**
    * Check if the file update time has changed.
    * 
    * @param   update   If true, the update time is update.
    * @ret     bool     Returns true if the file update time
    *                   has changed.
    */
   bool  updated(bool update = true);
   
   /**
    * Checks that the file still exists at the current path.
    */
   bool  exists();
   
   void        setName(std::string name);
   std::string getName() const;
   
   void        setBasePath(std::string path);
   std::string getBasePath() const;
   
   void removeChildren(std::vector<FileNode> &list);
   void addChildren(std::vector<FileNode> &list);
   
   // Ownership
   std::string user;
   std::string group;
   
   // Permissions
   static const perm_t Read;
   static const perm_t Write;
   static const perm_t Execute;
   
   perm_t   ownerPermissions;
   perm_t   groupPermissions;
   perm_t   otherPermissions;
   
   // File type
   enum FileType
   {
      None,
      Directory,
      Regular,
      SymLink,
      Socket,
      NamedPipe,
      CharDev,
      BlockDev
   };
   FileType type;
   
   // If this is a directory, it has children.
   std::vector<FileNode> children;
   
   void                 setFullPath(const std::string &path);
   std::string          fullPath()  const;
   static std::string   FullPath(const std::string &path, 
                                 const std::string &name);
   
   /**
    * For two directory nodes, Diff finds the files that have been added from
    * old to current, and the files that have been removed.  Returns false
    * if the compared nodes are not directories or they do not have the same
    * path.
    */
   static bool Diff(std::vector<FileNode> &added,
                    std::vector<FileNode> &removed,
                    const FileNode &old,
                    const FileNode &current,
                    const FileNameList &ignoreNames);
   
   static void Print(const FileNode &node, bool recurse = true);
   
   static bool Traverse(const std::string &path,
                        FileNode    &list, 
                        const FileNameList &ignoreNames,
                        bool  recurse = false);
   
   static bool Stat(std::string path, FileNode &list);
   
private:
   
   // Name and path
   std::string name;
   std::string basePath;
   
   struct FileTimes* m_pFileTimes;
   
   friend class FsHelper;
};
typedef std::vector<FileNode>::iterator  file_node_iter_t;
typedef std::vector<FileNode>::const_iterator  const_file_node_iter_t;


#endif // FILE_NODE_H