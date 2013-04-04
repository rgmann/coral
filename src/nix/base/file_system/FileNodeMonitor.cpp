
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
      if (node.type == FileNode::Directory)
      {
         //printf("Name: %d %s\n", locIterCount++, it->first.c_str());
         sync(node);
      }
   }
   
   //diff(m_BaseNode);
}

//------------------------------------------------------------------------------
FileNodeList FileNodeMonitor::getAddedNodes(bool fresh)
{
   FileNodeList addedList;
   std::vector<FileNode>::iterator it;
   
   it = m_AddedNodes.begin();
   for (; it < m_AddedNodes.end(); ++it)
   {
      addedList.push_back(*it);
   }
   
   // Clear the local list.
   m_AddedNodes.clear();
   
   return addedList;
}

//------------------------------------------------------------------------------
FileNodeList FileNodeMonitor::getRemovedNodes(bool fresh)
{
   FileNodeList removedList;
   std::vector<FileNode>::iterator it;
   
   it = m_RemovedNodes.begin();
   for (; it < m_RemovedNodes.end(); ++it)
   {
      removedList.push_back(*it);
   }
   
   return removedList;
}

//------------------------------------------------------------------------------
FileNodeList FileNodeMonitor::getModifiedNodes(bool fresh)
{
   FileNodeList modifiedList;
   std::vector<FileNode*>::iterator it;
   
   it = m_ModifiedNodes.begin();
   for (; it < m_ModifiedNodes.end(); ++it)
   {
      modifiedList.push_back(*(*it));
   }
   
   return modifiedList;
}

//------------------------------------------------------------------------------
FileNodeList FileNodeMonitor::getChangedNodes(bool fresh)
{
   FileNodeList changedList;
   std::vector<FileNode*>::iterator it;
   
   it = m_ChangedNodes.begin();
   for (; it < m_ChangedNodes.end(); ++it)
   {
      changedList.push_back(*(*it));
   }
   
   return changedList;
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
         }
      }
      
      // Find removed, modified, and changed files by recursing.
      it = base.children.begin();
      for (; it < base.children.end(); ++it)
      {
         //printf("base: %s, ov: %s\n", base.fullPath().c_str(), it->fullPath().c_str());
         // If the node does not exist any longer, remove
         // it and add it to the removed node list.
         if (!it->exists())
         {
            m_RemovedNodes.push_back(*it);
            it = base.children.erase(it);
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
         m_ModifiedNodes.push_back(&base);
      }
      if (base.updated())
      {
         m_ChangedNodes.push_back(&base);
      }
   }

}

//------------------------------------------------------------------------------
void FileNodeMonitor::registerNodes(FileNode &node, bool recurse)
{
   if (m_NodeLut.count(node.fullPath()) == 0)
   {
      m_NodeLut[node.fullPath()] = node;
   }
   
   if (node.type == FileNode::Directory && recurse)
   {
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
   if (m_NodeLut.count(fullPath) > 0)
   {
      return true;
   }
   
   return false;
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
   
   // Add the node to the new node list
   m_AddedNodes.push_back(newNode);
}
