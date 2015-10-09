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



#include <iostream>
#include "FileNodeMonitor.h"


//------------------------------------------------------------------------------
FileNodeMonitor::FileNodeMonitor()
{
   m_IgnoreList.push_back(".");
   m_IgnoreList.push_back("..");
}

//------------------------------------------------------------------------------
FileNodeMonitor::~FileNodeMonitor()
{
}

//------------------------------------------------------------------------------
void FileNodeMonitor::setBase(const std::string &basePath)
{
   FileNode::Traverse(basePath, m_BaseNode, m_IgnoreList, true);
   registerNodes(m_BaseNode);
   FileNode::Print(m_BaseNode);
}

//------------------------------------------------------------------------------
void FileNodeMonitor::setBase(FileNode &baseNode)
{
   m_BaseNode = baseNode;
   registerNodes(m_BaseNode);
}

//------------------------------------------------------------------------------
void FileNodeMonitor::refresh()
{
   std::map<std::string, FileNode>::iterator it;
   //int locIterCount = 0;
   
   it = m_NodeLut.begin();
   for (; it != m_NodeLut.end(); ++it)
   {
      FileNode node = it->second;
      
      // If the node is a directory and it was modified or updated,
      // then sync its contents.
      //if (node.type == FileNode::Directory)
      //{
         //printf("Name: %d %s\n", locIterCount++, it->first.c_str());
      //   sync(node);
      //}
      
      // Map only contains directories.
      syncDirectory(it->second);
   }
   
   // Add/remove directories. We cannot do this while iterating.
   FileNodeList::iterator lDirIt = mAddedDirList.begin();
   for (; lDirIt != mAddedDirList.end(); lDirIt++)
   {
      std::cout << "ADDING " << lDirIt->fullPath() << std::endl;
      m_NodeLut[lDirIt->fullPath()] = *lDirIt;
   }
   mAddedDirList.clear();
   
   lDirIt = mRemovedDirList.begin();
   for (; lDirIt != mRemovedDirList.end(); lDirIt++)
   {
      std::cout << "REMOVING " << lDirIt->fullPath() << std::endl;
      m_NodeLut.erase(lDirIt->fullPath());
   }
   mRemovedDirList.clear();
}

//------------------------------------------------------------------------------
bool FileNodeMonitor::getDelta(FileSysDelta &delta, int nTimeoutMs)
{
   return m_deltaQueue.pop(delta, nTimeoutMs);
}

//------------------------------------------------------------------------------
void FileNodeMonitor::sync(FileNode &base)
{
   if (base.type == FileNode::Directory)
   {
      FileNode          locNode;
      FileNodeList      locNewNodes;
      file_node_iter_t  it;
      
      // If the folder has not been modified or updated,
      // then we know that none of the contents have changed.
      if (!base.modified() && !base.updated())
      {
         return;
      }
      
      // Find the changed, modified, added, and removed files in this dir:
      
      
      // Find the added files:
      // Right now its easiest to just throw away the children and
      // re-traverse the tree.
      FileNode::Traverse(base.fullPath(), locNode, m_IgnoreList, true);
      //FileNode::Print(locNode);
      
      //printf("BASE:\n");
//      FileNode::Print(base);
//      printf("LOCNODE:\n");
//      FileNode::Print(locNode);
      it = locNode.children.begin();
      for (; it < locNode.children.end(); ++it)
      {
         //printf("base: %s, new: %s\n", base.fullPath().c_str(), it->fullPath().c_str());
         if (!nodeExistsForPath(it->fullPath()))
         {
            //printf("New: %s\n", it->fullPath().c_str());
            addNewNode(base, *it);
            pushDelta(Added, *it);
         }
      }
      
      // Find removed, modified, and changed files by recursing.
      it = base.children.begin();
      for (; it < base.children.end(); ++it)
      {
         printf("base: %s, ov: %s\n", base.fullPath().c_str(), it->fullPath().c_str());
         // If the node does not exist any longer, remove
         // it and add it to the removed node list.
         if (!it->exists())
         {
            it = base.children.erase(it);
            pushDelta(Removed, *it);
         }
         // Otherwise recurse...
         else if (!it->inIgnoreList(m_IgnoreList))
         {
            sync(*it);
         }
      }
   }
   else
   {
      if (base.modified())
      {
         pushDelta(Modified, base);
      }
      if (base.updated())
      {
         pushDelta(Updated, base);
      }
   }
}

//------------------------------------------------------------------------------
void FileNodeMonitor::syncDirectory(FileNode &oldDir)
{
   FileNode lCurrentDir;
   FileNodeList lAddedNodes;
   FileNodeList lRemovedNodes;
   
   if (!oldDir.exists())
   {
      mRemovedDirList.push_back(oldDir);
   }
   
   // Get the new state of the directory so that we can find out what has been
   // added or removed to the directory.  To eliminate overlapping updates
   // don't recurse.  For example, if this directory is tranversed recursively
   // and then this directories parent is traversed recursively, we are doing
   // unnecessary work.
   FileNode::Traverse(oldDir.fullPath(), lCurrentDir, m_IgnoreList, false);
   
   if (FileNode::Diff(lAddedNodes, lRemovedNodes, 
                      oldDir, lCurrentDir, m_IgnoreList))
   {
      // Find removed nodes
      FileNodeList::iterator it = lRemovedNodes.begin();
      for (; it != lRemovedNodes.end(); it++)
      {
         pushDelta(Removed, *it);
         
         // Removed by test at beginning.
         //if (it->type == FileNode::Directory) mRemovedDirQueue.push_back(*it);
      }
      oldDir.removeChildren(lRemovedNodes);
      
      // Out of nodes that have not been removed, find modified/updated nodes.
      it = oldDir.children.begin();
      for (; it != oldDir.children.end(); it++)
      {
         if (it->type == FileNode::Directory) continue;
         
         if (it->modified())
         {
            pushDelta(Modified, *it);
         }
         if (it->updated())
         {
            pushDelta(Updated, *it);
         }
      }
      
      // Add new nodes.
      it = lAddedNodes.begin();
      oldDir.addChildren(lAddedNodes);
      for (; it != lAddedNodes.end(); it++)
      {
         //m_AddedNodes.push_back(*it);
         pushDelta(Added, *it);
         
         if (it->type == FileNode::Directory) mAddedDirList.push_back(*it);
      }
   }
}

//------------------------------------------------------------------------------
void FileNodeMonitor::registerNodes(FileNode &node, bool recurse)
{
   //if (m_NodeLut.count(node.fullPath()) == 0)
//   {
//      m_NodeLut[node.fullPath()] = node;
//   }
   
   if (node.type == FileNode::Directory && recurse)
   {
      if (m_NodeLut.count(node.fullPath()) == 0)
      {
         m_NodeLut[node.fullPath()] = node;
      }
      
      std::vector<FileNode>::iterator it = node.children.begin();
      
      for (; it < node.children.end(); ++it)
      {
         registerNodes(*it);
      }
   }
}

//------------------------------------------------------------------------------
bool FileNodeMonitor::nodeExistsForPath(const std::string &fullPath)
{
   return (m_NodeLut.count(fullPath) > 0);
}

//------------------------------------------------------------------------------
void FileNodeMonitor::addNewNode(FileNode &parent, FileNode &newNode)
{
   //FileNode*   pNode = NULL;
   
   // Add the node to the parent's child list
   parent.children.push_back(newNode);
   
   if (parent.children.empty())
   {
      return;
   }
   //pNode = &parent.children.at(parent.children.size() - 1);
   
   // Add the node to the map.
   m_NodeLut[newNode.fullPath()] = newNode;//pNode;
}

//------------------------------------------------------------------------------
void FileNodeMonitor::pushDelta(DeltaType type, FileNode &node)
{
   FileSysDelta l_delta;
   
   l_delta.type = type;
   l_delta.node = node;
   
   m_deltaQueue.push(l_delta);
}
