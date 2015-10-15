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



#ifndef RSYNC_FS_INTERFACE_H
#define RSYNC_FS_INTERFACE_H

#include <boost/filesystem.hpp>

namespace coral {
namespace rsync {

class FileSystemInterface {
public:

   FileSystemInterface(const boost::filesystem::path& swapPath = ".rsync_swap",
                      const boost::filesystem::path& stagePath = ".rsync_stage");
   ~FileSystemInterface();

   //
   // Set the RsyncNode root directory. Files cannot be accessed above this
   // directory. If the path does not exist, false is returned.
   //
   bool setRoot( const boost::filesystem::path& root );

   //
   // Check whether the specified file exists.
   //
   bool exists( const boost::filesystem::path& path ) const;

   //
   // Open a file for writing. Returns false if the path does not exist or on
   // failure to open the file stream.
   //
   bool open( const boost::filesystem::path& path, std::ofstream& stream ) const;

   //
   // Open a file for reading. Returns false if the path does not exist or on
   // failure to open the file stream.
   //
   bool open( const boost::filesystem::path& path, std::ifstream& stream ) const;

   //
   // Rather than writing directly into the destination file, we minimize
   // the chance for a write conflict by writing to a stage file and then 
   // swapping the inodes.
   //
   bool stage(const boost::filesystem::path& destination,
             boost::filesystem::path& stagePath,
             std::ofstream& stream);

   //
   // Swap the paths for two inodes.
   //
   bool swap(const boost::filesystem::path& source,
            const boost::filesystem::path& destination);

   //
   // Delete a file within the root heirarchy. The path is relative to
   // configured root.
   //
   bool remove( const boost::filesystem::path& path ) const;

   //
   // 
   //
   bool touch( const boost::filesystem::path& path ) const;

private:

   FileSystemInterface( const FileSystemInterface& );
   FileSystemInterface& operator= ( const FileSystemInterface& );

   bool validate() const;

   /**
   * TODO: Allowed?  This could be used to find out base path.
   */
   boost::filesystem::path absolutePath(
      const boost::filesystem::path& relativePath ) const;

private:

   bool root_exists_;

   boost::filesystem::path swap_path_;

   boost::filesystem::path stage_path_;

   // Absolute path to rsync root directory.  Rsync does not know about
   // anything outside this path.
   boost::filesystem::path root_path_;

   int last_swap_file_index_;
   int last_stage_file_index_;

   friend class RsyncNode;
};

} // End namespace rsync
} // End namespace coral

#endif // RSYNC_FS_INTERFACE_H
