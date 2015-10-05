#include <fstream>
#include <sstream>
#include <boost/system/error_code.hpp>
#include "Log.h"
#include "FileSystemInterface.h"

using namespace boost::filesystem;
using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
FileSystemInterface::FileSystemInterface(
  const boost::filesystem::path& swap_path,
  const boost::filesystem::path& stage_path
)
: root_exists_( false )
, swap_path_  ( swap_path )
, stage_path_ ( stage_path )
, last_swap_file_index_(0)
, last_stage_file_index_(0)
{
}

//----------------------------------------------------------------------------
FileSystemInterface::~FileSystemInterface()
{
}

//----------------------------------------------------------------------------
bool FileSystemInterface::setRoot( const path& root_path )
{
   bool status = false;
   boost::system::error_code file_sys_error;

   root_exists_ = boost::filesystem::exists( root_path, file_sys_error );

   if ( root_exists_ )
   {
      status     = true;
      root_path_ = root_path;

      // Check that the swap directory exists.  If not, create it.
      bool path_exists = boost::filesystem::exists(
         absolutePath( swap_path_ ),
         file_sys_error
      );

      if ( file_sys_error.value() != boost::system::errc::success )
      {
         log::debug("FileSystemInterface::setRoot: %s - %s\n",
                    swap_path_.string().c_str(),
                    file_sys_error.message().c_str());
      }
      if ( path_exists == false )
      {
         log::debug("Creating %s\n", swap_path_.string().c_str());
         status &= create_directory(absolutePath(swap_path_), file_sys_error);
         if ( file_sys_error.value() != boost::system::errc::success )
         {
            log::error("FileSystemInterface::setRoot: Error - %s\n",
                        file_sys_error.message().c_str());
            status = false;
         }
      }

      // Check that the stage directory exists.  If not, create it.
      path_exists = boost::filesystem::exists(
         absolutePath(stage_path_),
         file_sys_error
      );

      if ( file_sys_error.value() != boost::system::errc::success )
      {
         log::debug("FileSystemInterface::setRoot: %s - %s\n",
                    stage_path_.string().c_str(),
                    file_sys_error.message().c_str());
      }
      if (path_exists == false)
      {
         status &= create_directory(absolutePath(stage_path_), file_sys_error);
         if (file_sys_error.value() != boost::system::errc::success)
         {
            log::error("FileSystemInterface::setRoot: Error - %s\n",
                       file_sys_error.message().c_str());
            status = false;
         }
      }
   }
   else
   {
      log::error("FileSystemInterface::setRoot: %s - %s\n",
         root_path.string().c_str(),
         file_sys_error.message().c_str());
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::exists( const boost::filesystem::path& path ) const
{
   bool path_exists = false;
   boost::system::error_code file_sys_error;

   if ( validate() )
   {
      path_exists = boost::filesystem::exists(
         absolutePath( path ),
         file_sys_error
      );
   }

   return path_exists;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::open(
  const boost::filesystem::path& path,
  std::ofstream& stream
) const
{
   bool status = false;

   if ( validate() )
   {
      stream.open( absolutePath( path ).c_str(), std::ofstream::binary );

      if ( stream.fail() )
      {
         log::debug(
            "FileSystemInterface::open: "
            "Failed to open output stream at %s\n",
            path.string().c_str());
      }

      status = stream.is_open();
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::open(
  const boost::filesystem::path& path,
  std::ifstream& stream
) const
{
   bool status = false;

   if ( validate() )
   {
      if ( exists( path ) )
      {
         stream.open( absolutePath( path ).c_str(), std::ofstream::binary );

         if ( ( status = ( stream.is_open() && stream.good() ) ) == false )
         {
            log::error(
               "FileSystemInterface::open: "
               "Failed to open output stream at %s\n",
               path.string().c_str());
         }
      }
      else
      {
         log::error("FileSystemInterface: Input file does not exist.\n");
      }
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::stage(
   const boost::filesystem::path&  destination,
   boost::filesystem::path&        stage_path,
   std::ofstream&                  stream
)
{
   bool status = false;

   if ( validate() )
   {
      std::stringstream stage_filename;

      stage_filename << ++last_swap_file_index_ << "_stage." 
                     << destination.filename().string();

      stage_path = stage_path_ / stage_filename.str();

      status = open( stage_path, stream );
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::swap(
   const boost::filesystem::path& source,
   const boost::filesystem::path& destination)
{
   bool status = false;

   if ( validate() )
   {
      // Source and destination must exist.
      if ( exists( source ) && exists( destination ) )
      {
         std::stringstream swap_filename;

         swap_filename << ++last_swap_file_index_ << "_swap."
                       << destination.filename().string();

         boost::system::error_code file_sys_error;
         boost::filesystem::path swap_path = swap_path_ / swap_filename.str();

         boost::filesystem::rename(
            absolutePath( destination ),
            absolutePath( swap_path ),
            file_sys_error
         );

         status = ( file_sys_error.value() == boost::system::errc::success );
         if ( !status )
         {
            log::error("FileSystemInterface::swap: "
                      "Error renaming %s to %s - %s\n",
                      destination.string().c_str(),
                      swap_path.string().c_str(),
                      file_sys_error.message().c_str());
         }

         if ( status )
         {
            boost::filesystem::rename(
               absolutePath( source ),
               absolutePath( destination ),
               file_sys_error
            );

            status = ( file_sys_error.value() == boost::system::errc::success );
            if ( status == false )
            {
               log::error(
                  "FileSystemInterface::swap: Error renaming %s to %s - %s\n",
                  source.c_str(),
                  destination.c_str(),
                  file_sys_error.message().c_str());
            }
         }

         if (status)
         {
            boost::filesystem::rename(
               absolutePath( swap_path ),
               absolutePath( source ),
               file_sys_error
            );

            status = ( file_sys_error.value() == boost::system::errc::success );
            if ( status == false )
            {
               log::error(
                  "FileSystemInterface::swap: Error renaming %s to %s - %s\n",
                  swap_path.string().c_str(),
                  source.string().c_str(),
                  file_sys_error.message().c_str());
            }
         }
      }
      else
      {
         log::error(
            "FileSystemInterface::swap: "
            "Source and destination must exist.\n" );
      }
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::remove( const boost::filesystem::path& path ) const
{
   bool status = false;

   if ( validate() )
   {
      if ( exists( path ) )
      {
         boost::system::error_code file_sys_error;

         status = boost::filesystem::remove(
            absolutePath( path ),
            file_sys_error
         );

         status = ( file_sys_error.value() == boost::system::errc::success );
         if ( status == false )
         {
            log::error(
               "FileSystemInterface::remove: %s\n",
               file_sys_error.message().c_str());
         }
      }
      else
      {
         log::error("FileSystemInterface::remove: %s not found.\n",
                    path.string().c_str());
      }
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::touch( const boost::filesystem::path& path ) const
{
   bool status = false;

   if ( validate() )
   {
      if ( ( status = exists( path ) ) == false )
      {
         std::ofstream fs( absolutePath( path ).c_str(), std::ofstream::binary );

         status = ( fs.is_open() && fs.good() );

         fs.close();

         status = ( status && exists( path ) );
      }
   }

   return status;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::validate() const
{
   if ( root_exists_ == false )
   {
      log::debug("FileSystemInterface: Root not set.\n");
   }

   return root_exists_;
}

//----------------------------------------------------------------------------
boost::filesystem::path FileSystemInterface::absolutePath(
   const boost::filesystem::path& relative_path
) const
{
   return root_path_ / relative_path;
}

