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
   
   bool inIgnoreList(const FileNameList &names);
   
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
   std::string getName();
   
   void        setBasePath(std::string path);
   std::string getBasePath();
   
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