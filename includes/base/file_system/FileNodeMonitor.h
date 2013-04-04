#ifndef FILE_NODE_MON_H
#define FILE_NODE_MON_H

#include "FileNode.h"
#include <map>

typedef std::vector<FileNode*> FileNodeRefList;
typedef std::vector<FileNode>  FileNodeList;

class FileNodeMonitor
{
public:
   
   FileNodeMonitor();
   
   ~FileNodeMonitor();
   
   void           setBase(const std::string &basePath);
   
   void           setBase(FileNode &baseNode);
   
   void  refresh();
   
   FileNodeList   getAddedNodes(bool fresh = false);
   
   /**
    * Finds all nodes that no longer exist.
    * Uses directory mtime to speed up the process.
    * Renamed files are currently considered removed nodes.
    */
   FileNodeList   getRemovedNodes(bool fresh = false);
   
   /**
    * mtime changes when you write to the file. 
    * It is the age of the data in the file. 
    * Whenever mtime changes, so does ctime.
    */
   FileNodeList   getModifiedNodes(bool fresh = false);
   
   /**
    * But ctime changes a few extra times. 
    * For example, it will change if you change 
    * the owner or the permissions on the file.
    */
   FileNodeList   getChangedNodes(bool fresh = false);
   
private:
   
   void sync(FileNode &base);
   
   bool nodeExistsForPath(const std::string &fullPath);
   
   void addNewNode(FileNode &parent, FileNode &newNode);
   
   void registerNodes(FileNode &node, bool recurse = true);
   
private:
   
   FileNode          m_BaseNode;
   //FileNode          m_BaseNode;
   
   FileNodeList      m_AddedNodes;
   FileNodeList      m_RemovedNodes;
   FileNodeRefList   m_ModifiedNodes;
   FileNodeRefList   m_ChangedNodes;
   
   // List of file/dir names to ignore when traversing.
   FileNameList      m_IgnoreList;
   
   // If a path does not appear in the LUT,
   // then it is a new file.
   std::map<std::string, FileNode> m_NodeLut;
};

#endif // FILE_NODE_MON_H