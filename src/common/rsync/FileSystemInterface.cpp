#include <sstream>
#include <boost/system/error_code.hpp>
#include "Log.h"
#include "FileSystemInterface.h"

using namespace boost::filesystem;
using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
FileSystemInterface::
FileSystemInterface(const boost::filesystem::path& swapPath,
                    const boost::filesystem::path& stagePath)
: mbRootExists(false)
, mSwapPath(swapPath)
, mStagePath(stagePath)
, mnLastSwapFileIndex(0)
, mnLastStageFileIndex(0)
{
}

//----------------------------------------------------------------------------
FileSystemInterface::~FileSystemInterface()
{
}

//----------------------------------------------------------------------------
bool FileSystemInterface::setRoot(const path& rootPath)
{
  bool lbSuccess = false;
  boost::system::error_code lErrorCode;

  mbRootExists = boost::filesystem::exists(rootPath, lErrorCode);

  if (mbRootExists)
  {
    mRootPath = rootPath;
    //mSwapPath = mRootPath / mSwapPath;
    //mStagePath = mRootPath / mStagePath;

    // Check that the swap directory exists.  If not, create it.
    bool lbExists = boost::filesystem::exists(absolutePath(mSwapPath), lErrorCode);
    if (lErrorCode.value() != boost::system::errc::success)
    {
      log::debug("FileSystemInterface::setPath: %s - %s\n",
                 mSwapPath.string().c_str(),
                 lErrorCode.message().c_str());
    }
    if (lbExists == false)
    {
      log::debug("Creating %s\n", mSwapPath.string().c_str());
      lbSuccess = create_directory(absolutePath(mSwapPath), lErrorCode);
      if (lErrorCode.value() != boost::system::errc::success)
      {
        log::error("FileSystemInterface::setPath: Error - %s\n",
                   lErrorCode.message().c_str());
      }
    }

    // Check that the stage directory exists.  If not, create it.
    lbExists = boost::filesystem::exists(absolutePath(mStagePath), lErrorCode);
    if (lErrorCode.value() != boost::system::errc::success)
    {
      log::debug("FileSystemInterface::setPath: %s - %s\n",
                 mStagePath.string().c_str(),
                 lErrorCode.message().c_str());
    }
    if (lbExists == false)
    {
      lbSuccess = create_directory(absolutePath(mStagePath), lErrorCode);
      if (lErrorCode.value() != boost::system::errc::success)
      {
        log::error("FileSystemInterface::setPath: Error - %s\n",
                   lErrorCode.message().c_str());
      }
    }
  }
  else
  {
    log::error("FileSystemInterface::setPath: %s\n",
               lErrorCode.message().c_str());
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::exists(const boost::filesystem::path& path) const
{
  bool lbPathExists = false;
  boost::system::error_code lErrorCode;

  if (validate())
  {
    lbPathExists = boost::filesystem::exists(absolutePath(path), lErrorCode);
  }

  return lbPathExists;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::
open(const boost::filesystem::path& path, std::ofstream& stream) const
{
  bool lbSuccess = false;

  if (validate())
  {
    stream.open(absolutePath(path).string().c_str(),
                std::ofstream::binary);

    if (stream.fail())
    {
      log::debug("FileSystemInterface::open: Failed to open output stream at %s\n",
                 path.string().c_str());
    }

    lbSuccess = stream.is_open();
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::
open(const boost::filesystem::path& path, std::ifstream& stream) const
{
  bool lbSuccess = false;

  if (validate())
  {
    if (exists(path))
    {
      stream.open(absolutePath(path).string().c_str(),
                  std::ofstream::binary);

      if (stream.fail())
      {
        log::error("FileSystemInterface::open: Failed to open output stream at %s\n",
                   path.string().c_str());
      }

      lbSuccess = stream.is_open();
    }
    else
    {
      log::error("FileSystemInterface: Input file does not exist.\n");
    }
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::
stage(const boost::filesystem::path& destination,
            boost::filesystem::path& stagePath,
            std::ofstream& stream)
{
  bool lbSuccess = false;

  if (validate())
  {
    std::stringstream lStageFilename;
    lStageFilename << ++mnLastSwapFileIndex << "_stage." 
                   << destination.filename().string();
    stagePath = mStagePath / lStageFilename.str();
    
    lbSuccess = open(stagePath, stream);
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::
swap(const boost::filesystem::path& source,
     const boost::filesystem::path& destination)
{
  bool lbSuccess = false;

  if (validate())
  {
    // Source and destination must exist.
    if (exists(source) && exists(destination))
    {
      std::stringstream lSwapFileName;
      lSwapFileName << ++mnLastSwapFileIndex << "_swap."
                    << destination.filename().string();

      boost::system::error_code lErrorCode;
      path lSwapPath = mSwapPath / lSwapFileName.str();

      rename(absolutePath(destination), absolutePath(lSwapPath), lErrorCode);
      lbSuccess = (lErrorCode.value() == boost::system::errc::success);
      if (!lbSuccess)
      {
        log::error("FileSystemInterface::swap: "
                   "Error renaming %s to %s - %s\n",
                   destination.string().c_str(),
                   lSwapPath.string().c_str(),
                   lErrorCode.message().c_str());
      }

      if (lbSuccess)
      {
        rename(absolutePath(source), absolutePath(destination), lErrorCode);
        lbSuccess = (lErrorCode.value() == boost::system::errc::success);
        if (!lbSuccess)
        {
          log::error("FileSystemInterface::swap: "
                     "Error renaming %s to %s - %s\n",
                     source.string().c_str(),
                     destination.string().c_str(),
                     lErrorCode.message().c_str());
        }
      }

      if (lbSuccess)
      {
        rename(absolutePath(lSwapPath), absolutePath(source), lErrorCode);
        lbSuccess = (lErrorCode.value() == boost::system::errc::success);
        if (!lbSuccess)
        {
          log::error("FileSystemInterface::swap: "
                     "Error renaming %s to %s - %s\n",
                     lSwapPath.string().c_str(),
                     source.string().c_str(),
                     lErrorCode.message().c_str());
        }
      }
    }
    else
    {
      log::error("FileSystemInterface::swap: "
                 "Source and destination must exist.\n");
    }
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::remove(const boost::filesystem::path& path) const
{
  bool lbSuccess = false;

  if (validate())
  {
    if (exists(absolutePath(path)))
    {
      boost::system::error_code lErrorCode;
      lbSuccess = boost::filesystem::remove(absolutePath(path), lErrorCode);

      lbSuccess = (lErrorCode.value() == boost::system::errc::success);
      if (!lbSuccess)
      {
        log::error("FileSystemInterface::remove: %s\n",
                   lErrorCode.message().c_str());
      }
    }
    else
    {
      log::error("FileSystemInterface::remove: %s not found.\n",
                 path.string().c_str());
    }
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
bool FileSystemInterface::validate() const
{
  if (!mbRootExists)
  {
    log::debug("FileSystemInterface: Root not set.\n");
  }

  return mbRootExists;
}

//----------------------------------------------------------------------------
boost::filesystem::path FileSystemInterface::
absolutePath(const boost::filesystem::path& relativePath) const
{
  return mRootPath / relativePath;
}

