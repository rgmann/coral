#ifndef  RSYNC_TEST_HELPER_H
#define  RSYNC_TEST_HELPER_H

#include <vector>
#include <boost/filesystem.hpp>

namespace rsync_test_helper {

   typedef  std::vector<boost::filesystem::path>  FileList;

   inline void GetFilesListing( const char* pPath, FileList& listing, bool bRecursive = false )
   {
      boost::filesystem::path directory( pPath );
      boost::filesystem::directory_iterator endIter;

      // std::cout << "PATH: " << pPath << std::endl;
      if ( boost::filesystem::exists( directory ) &&
           boost::filesystem::is_directory( directory ) )
      {
        for( boost::filesystem::directory_iterator dirIter( directory ) ; dirIter != endIter ; ++dirIter)
        {
          if ( boost::filesystem::is_regular_file( dirIter->status() ) )
          {
            // std::cout << dirIter->path().string() << std::endl;
            listing.push_back( dirIter->path() );
          }
          else if ( bRecursive )
          {
            GetFilesListing( dirIter->path().c_str(), listing, bRecursive );
          }
        }
      }
   }

   inline void DeleteContents( const char* pPath )
   {
      FileList listing;

      GetFilesListing( pPath, listing, true );

      FileList::iterator file = listing.begin();
      for (; file != listing.end(); file++ )
      {
        if ( boost::filesystem::exists( *file ) )
        {
          boost::filesystem::remove( *file );
        }
      }
   }

   inline void CopyContents( const char* pSrcPath, const char* pDstPath )
   {
      boost::system::error_code error;
      FileList listing;

      GetFilesListing( pSrcPath, listing );

      FileList::iterator file = listing.begin();
      for (; file != listing.end(); file++ )
      {
         if ( boost::filesystem::exists( *file ) )
         {
            boost::filesystem::copy_file(
               *file,
               boost::filesystem::path( pDstPath ) / file->filename(),
               boost::filesystem::copy_option::none,
               error );

            if ( error != boost::system::errc::success )
            {
               std::cout << "Failed to copy " << file->string()
                         << " to " << boost::filesystem::path( pDstPath ) << std::endl;
            }
         }
      }
   }

   inline bool CheckEqual(
      const boost::filesystem::path& rSource,
      const boost::filesystem::path& rDestination )
   {
      bool bEqual = true;

      std::ifstream sourceStream(
        rSource.string().c_str(), std::ifstream::in | std::ifstream::binary );
      std::ifstream destinationStream(
        rDestination.string().c_str(), std::ifstream::in | std::ifstream::binary );

      if ( sourceStream.is_open() && destinationStream.is_open() )
      {
        char sourceChar = 0;
        char destinationChar = 0;
        int  nByteIndex = 0;

        while ( bEqual )
        {
          sourceStream.get( sourceChar );
          destinationStream.get( destinationChar );

          if ( sourceStream.eof() && destinationStream.eof() )
          {
            // Successful completion.
            break;
          }
          else if ( !sourceStream.good() || !destinationStream.good() )
          {
            // One file reached EOF while the other did not.
            bEqual = false;
          }
          else
          {
            if ( sourceChar != destinationChar )
            {
              bEqual = false;
            }
          }

          if ( !bEqual )
          {
            std::cout << "Mismatch between " << rSource
                      << " and " << rDestination
                      << " at " << nByteIndex << std::endl;
          }

          nByteIndex++;
        }
      }
      else
      {
        bEqual = false;
        std::cout << "Failed to open " << rSource << " or " << rDestination << std::endl;
      }

      sourceStream.close();
      destinationStream.close();

      return bEqual;
   }

} // namespace RsyncTestHelper

#endif // RSYNC_TEST_HELPER_H
