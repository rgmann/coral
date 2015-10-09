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



#include "FileNode.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <map>

const perm_t FileNode::Read = 0x04;
const perm_t FileNode::Write = 0x02;
const perm_t FileNode::Execute = 0x01;

struct FileTimes
{
   time_t   lastAccess;
   time_t   lastModification;
   time_t   lastUpdate;
};

//------------------------------------------------------------------------------
FileNode::FileNode()
{
   // Name and path
   name = "";
   basePath = "";
   
   // Ownership
   user = "";
   group = "";
   
   memset(&ownerPermissions, 0, sizeof(perm_t));
   memset(&groupPermissions, 0, sizeof(perm_t));
   memset(&otherPermissions, 0, sizeof(perm_t));
   
   type = None;
   
   m_pFileTimes = NULL;
   m_pFileTimes = new FileTimes();
   memset(&m_pFileTimes->lastAccess, 0, sizeof(time_t));
   memset(&m_pFileTimes->lastModification, 0, sizeof(time_t));
   memset(&m_pFileTimes->lastUpdate, 0, sizeof(time_t));
}

//------------------------------------------------------------------------------
FileNode::~FileNode()
{
   if (m_pFileTimes)
   {
      delete m_pFileTimes;
      m_pFileTimes = NULL;
   }
}

//------------------------------------------------------------------------------
FileNode::FileNode(const FileNode &node)
{
   m_pFileTimes = NULL;
   *this = node;
}

//------------------------------------------------------------------------------
FileNode & FileNode::operator=(const FileNode &rhs)
{
   if (this == &rhs)
   {
      return *this;
   }
   
   // Name and path
   name = rhs.name;
   basePath = rhs.basePath;
   
   // Ownership
   user = rhs.user;
   group = rhs.group;
   
   ownerPermissions = rhs.ownerPermissions;
   groupPermissions = rhs.groupPermissions;
   otherPermissions = rhs.otherPermissions;
   
   type = rhs.type;
   
   children = rhs.children;
   
   if (m_pFileTimes)
   {
      delete m_pFileTimes;
      m_pFileTimes = NULL;
   }
   
   m_pFileTimes = new FileTimes();
   if (m_pFileTimes && rhs.m_pFileTimes)
   {
      memcpy(m_pFileTimes, rhs.m_pFileTimes, sizeof(FileTimes));
   }
   
   return *this;
}

//------------------------------------------------------------------------------
bool FileNode::operator==(const FileNode &other) const
{
   if (name != other.name)
      return false;
   else if (basePath != other.basePath)
      return false;
   else if (user != other.user)
      return false;
   else if (group != other.group)
      return false;
   else if (ownerPermissions != other.ownerPermissions)
      return false;
   else if (groupPermissions != other.groupPermissions)
      return false;
   else if (otherPermissions != other.otherPermissions)
      return false;
   else if (type != other.type)
      return false;
   return true;
}

//------------------------------------------------------------------------------
bool FileNode::operator!=(const FileNode &other) const
{
   return !(*this == other);
}

//------------------------------------------------------------------------------
void FileNode::Print(const FileNode &node, bool recurse)
{
   if (node.type == None)
   {
      printf("FileNode::Print: Invalid node\n");
   }
   else if (node.type == Directory)
   {
      printf("%s\n", node.fullPath().c_str());
      
      if (recurse)
      {
         const_file_node_iter_t locNodeIter = node.children.begin();
         for (; locNodeIter < node.children.end(); ++locNodeIter)
         {
            Print(*locNodeIter);
         }
      }
   }
   else
   {
      printf("%s\n", node.fullPath().c_str());
   }
}

//------------------------------------------------------------------------------
bool FileNode::inIgnoreList(const FileNameList &names) const
{
   std::vector<std::string>::const_iterator it = names.begin();
   
   for (; it < names.end(); ++it)
   {
      if (*it == name)
      {
         return true;
      }
   }
   
   return false;
}

//------------------------------------------------------------------------------
bool FileNode::accessed(bool update)
{
   FileNode locNode;
   bool     locbChanged = false;
   
   if (!Stat(fullPath().c_str(), locNode))
   {
      return true;
   }
   
   if (locNode.m_pFileTimes->lastAccess != this->m_pFileTimes->lastAccess)
   {
      locbChanged = true;
      
      if (update)
      {
         this->m_pFileTimes->lastAccess = locNode.m_pFileTimes->lastAccess;
      }
   }
   
   return locbChanged;
}

//------------------------------------------------------------------------------
bool FileNode::modified(bool update)
{
   FileNode locNode;
   bool     locbChanged = false;
   
   if (!Stat(fullPath().c_str(), locNode))
   {
      printf("Stat failed\n");
      return true;
   }
   
   if (locNode.m_pFileTimes->lastModification != 
       this->m_pFileTimes->lastModification)
   {
      locbChanged = true;
      
      if (update)
      {
         this->m_pFileTimes->lastModification = 
            locNode.m_pFileTimes->lastModification;
      }
   }
   
   return locbChanged;
}

//------------------------------------------------------------------------------
bool FileNode::updated(bool update)
{
   FileNode locNode;
   bool     locbChanged = false;
   
   if (!Stat(fullPath().c_str(), locNode))
   {
      return true;
   }
   
   if (locNode.m_pFileTimes->lastUpdate != this->m_pFileTimes->lastUpdate)
   {
      locbChanged = true;
      
      if (update)
      {
         this->m_pFileTimes->lastUpdate = locNode.m_pFileTimes->lastUpdate;
      }
   }
   
   return locbChanged;
}

//------------------------------------------------------------------------------
bool FileNode::exists()
{
   FileNode locNode;
   
   if (!Stat(fullPath().c_str(), locNode))
   {
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
void FileNode::setName(std::string name)
{
   this->name = name;
}

//------------------------------------------------------------------------------
std::string FileNode::getName() const
{
   return name;
}

//------------------------------------------------------------------------------
void FileNode::setBasePath(std::string basePath)
{
   this->basePath = basePath;
}

//------------------------------------------------------------------------------
std::string FileNode::getBasePath() const
{
   return basePath;
}

//------------------------------------------------------------------------------
void FileNode::setFullPath(const std::string &fullPath)
{
   std::string locFullPath(fullPath);
   
   if (locFullPath.empty())
   {
      basePath = "";
      name = "";
   }
   else {
      // Finds a forward or backward slash.
      size_t lastSlashPos = locFullPath.find_last_of("/\\");
      
      // If the slash is the last character, remove it.
      if (lastSlashPos == (locFullPath.length() - 1))
      {
         locFullPath.erase(lastSlashPos);
         lastSlashPos = locFullPath.find_last_of("/\\");
      }
      
      if (lastSlashPos == std::string::npos)
      {
         basePath  = ".";
         name      = fullPath;
      }
      else
      {
         basePath = locFullPath.substr(0, lastSlashPos);
         name = locFullPath.substr(lastSlashPos + 1);
      }
   }

}

//------------------------------------------------------------------------------
std::string FileNode::fullPath() const
{
   return FullPath(basePath, name);
}

//------------------------------------------------------------------------------
std::string FileNode::FullPath(const std::string &path, 
                               const std::string &name)
{
   std::string locPath;
   
   if (!path.empty() && 
       path.at(path.length() - 1) != '/')
   {
      locPath = path + "/";
   }
   else
   {
      locPath = path;
   }

   return locPath + name;
}

//------------------------------------------------------------------------------
void FileNode::removeChildren(std::vector<FileNode> &list)
{
   std::map<std::string, FileNode> lNodeMap;
   
   std::vector<FileNode>::iterator lIt = list.begin();
   for (; lIt != list.end(); lIt++)
   {
      lNodeMap[lIt->fullPath()] = *lIt;
   }
   
   lIt = children.begin();
   while (lIt != children.end())
   {
      if (lNodeMap.count(lIt->fullPath()))
      {
         lIt = children.erase(lIt);
      }
      else
      {
         lIt++;
      }
   }
}

//------------------------------------------------------------------------------
void FileNode::addChildren(std::vector<FileNode> &list)
{
   std::vector<FileNode>::iterator lIt = list.begin();
   for (; lIt != list.end(); lIt++)
   {
      children.push_back(*lIt);
   }
}

//------------------------------------------------------------------------------
bool FileNode::Traverse(const std::string &path,
                        FileNode &node, const FileNameList &ignoreNames, 
                        bool recurse)
{
   DIR*  locpDir = NULL;
   struct dirent* locpFile = NULL;
   file_node_iter_t it;
   
   //node.path = path;
//   node.name = name;
   node.setFullPath(path);
   //printf("TRAVERSE: %s\n", node.fullPath().c_str());
   
   // Do a stat to determine if this node is a directory,
   // file, or neither.
   if (!Stat(node.fullPath(), node))
   {
      printf("FsHelper::GetFilesInDir: Failed to stat node\n");
      return false;
   }
   
   if (node.type != FileNode::Directory)
   {
      printf("FsHelper::GetFilesInDir: Not a directory\n");
      return false;
   }
   
   // If this is an ignored directory, then return.
   if (node.inIgnoreList(ignoreNames))
   {
      return false;
   }
   
   // Otherwise its a directory
   locpDir = opendir(node.fullPath().c_str());
   
   if (locpDir == NULL)
   {
      return false;
   }
   
   // Read list of subfile nodes into the node's "children" vector.
   // This way we don't try to open more than one directory at a time.
   while ((locpFile = readdir(locpDir)) != NULL)
   {
      FileNode locNode;
      
      // Populate the file properties
      locNode.basePath = node.fullPath();
      locNode.name = std::string(locpFile->d_name);
      //printf("P&N: %s, %s\n", locNode.basePath.c_str(), locNode.name.c_str());
      
      // Get the file stats
      if (Stat(locNode.fullPath(), locNode))
      {
         //printf("PbTrav: %s\n", locNode.fullPath().c_str());
         node.children.push_back(locNode);
      }
      else
      {
         //printf("Failed to stat child\n");
         closedir(locpDir);
         return false;
      }

   }
   
   // Close the directory
   closedir(locpDir);
   
   it = node.children.begin();
   for (; it < node.children.end(); ++it)
   {
      //FileNode locNode = *nodeIt;
      
      // If this is directory and recursion is enabled
      // then recurse.
      //printf("Parent: %s, Child: %s\n", FileNode::FullPath(node).c_str(), FileNode::FullPath(locNode).c_str());
      if (it->type == FileNode::Directory && !it->inIgnoreList(ignoreNames) && recurse)
      {
         //printf("RTrav: %s\n", it->fullPath().c_str());
         if (!Traverse(it->fullPath(), *it, ignoreNames, recurse))
         {
            return false;
         }
      }
   }
   
   //printf("END: path=%s, name=%s, fpath=%s\n", 
//          node.basePath.c_str(), node.name.c_str(), node.fullPath().c_str());
//   FileNode::Print(node);
   
   return true;
}

//------------------------------------------------------------------------------
bool FileNode::Stat(std::string path, FileNode &node)
{
   struct stat locStat;
   struct group *grp;
   struct passwd *pwd;
   
   if (stat(path.c_str(), &locStat) == -1)
   {
      node.type = FileNode::None;
      return false;
   }
   
   // Get the file type
   switch (locStat.st_mode & S_IFMT) {
      case S_IFBLK:  node.type = CharDev;   break;
      case S_IFCHR:  node.type = BlockDev;  break;
      case S_IFDIR:  node.type = Directory;  break;
      case S_IFIFO:  node.type = NamedPipe; break;
      case S_IFLNK:  node.type = SymLink;   break;
      case S_IFREG:  node.type = Regular;            break;
      case S_IFSOCK: node.type = Socket;                  break;
      default: break;
   }
   
   // If the path points to a regular file, then we already stat'd it
   // and can return.
   if ((node.type != Directory) && (node.type != None))
   {
      //...but first we should parse the path and name into separate fields.
      
      // Finds a forward or backward slash.
      size_t lastSlashPos = path.find_last_of("/\\");
      
      if (lastSlashPos == std::string::npos)
      {
         node.name = path;
      }
      else
      {
         node.basePath = path.substr(0, lastSlashPos);
         node.name = path.substr(lastSlashPos + 1);
      }
   }
   
   //printf("Stat'ing: %s, type = %d\n", FileNode::FullPath(node).c_str(), (int)node.type);
   
   // Get the group ownership
   if ((grp = getgrgid(locStat.st_gid)) != NULL)
   {
      node.group = std::string(grp->gr_name);
   }
   
   // Get the user ownership
   if ((pwd = getpwuid(locStat.st_uid)) != NULL)
   {
      node.user = std::string(pwd->pw_name);
   }
   
   // Get the owner permissions
   node.ownerPermissions = (locStat.st_mode & S_IRWXU) >> 8;
   
   // Get the group permissions
   node.groupPermissions = (locStat.st_mode & S_IRWXG) >> 4;
   
   // Get the other permissions
   node.otherPermissions = (locStat.st_mode & S_IRWXO);
   
   // Get file times
   node.m_pFileTimes->lastAccess = locStat.st_atime;
   node.m_pFileTimes->lastModification = locStat.st_mtime;
   node.m_pFileTimes->lastUpdate = locStat.st_ctime;
   
   return true;
}

//------------------------------------------------------------------------------
bool FileNode::Diff(std::vector<FileNode> &added,
                    std::vector<FileNode> &removed,
                    const FileNode &old,
                    const FileNode &current,
                    const FileNameList &ignoreNames)
{
   std::map<std::string, const FileNode*> lOldMap;
   std::map<std::string, const FileNode*>::iterator lOldMapIt;
   
   std::map<std::string, const FileNode*> lCurrentMap;
   std::map<std::string, const FileNode*>::iterator lCurrentMapIt;
   
   if (old.type != FileNode::Directory || current.type != FileNode::Directory)
   {
      return false;
   }
   
   if (old.fullPath() != current.fullPath())
   {
      return false;
   }
   
   std::vector<FileNode>::const_iterator lOldIt = old.children.begin();
   for (; lOldIt != old.children.end(); lOldIt++)
   {
      if (!lOldIt->inIgnoreList(ignoreNames))
      {
         lOldMap[lOldIt->getName()] = &(*lOldIt);
      }
   }
   
   std::vector<FileNode>::const_iterator lCurrentIt = current.children.begin();
   for (; lCurrentIt != current.children.end(); lCurrentIt++)
   {
      if (!lCurrentIt->inIgnoreList(ignoreNames))
      {
         lCurrentMap[lCurrentIt->getName()] = &(*lCurrentIt);
      }
   }
   
   lOldMapIt = lOldMap.begin();
   for (; lOldMapIt != lOldMap.end(); lOldMapIt++)
   {
      if (lCurrentMap.count(lOldMapIt->first) == 0)
      {
         removed.push_back(*lOldMapIt->second);
      }
   }
   
   lCurrentMapIt = lCurrentMap.begin();
   for (; lCurrentMapIt != lCurrentMap.end(); lCurrentMapIt++)
   {
      if (lOldMap.count(lCurrentMapIt->first) == 0)
      {
         added.push_back(*lCurrentMapIt->second);
      }
   }
   
   return true;
}
