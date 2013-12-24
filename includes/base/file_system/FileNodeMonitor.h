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
