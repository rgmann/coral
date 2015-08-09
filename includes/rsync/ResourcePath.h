#ifndef  RESOURCE_PATH_H
#define  RESOURCE_PATH_H


#include <string>
#include <boost/filesystem.hpp>
#include "BaseTypes.h"
#include "Serializable.h"


namespace liber {
namespace rsync {


class ResourcePath : public liber::netapp::Serializable {
public:

   ResourcePath();
   ResourcePath( const boost::filesystem::path&, bool remote );
   virtual ~ResourcePath() {};

   const boost::filesystem::path& path() const;
   void setPath( const boost::filesystem::path& path );

   bool remote() const;
   void setRemote( bool remote );

protected:

   void pack(liber::netapp::SerialStream& rCtor);
   void pack(liber::netapp::SerialStream& rCtor) const;
   bool unpack(liber::netapp::SerialStream& rDtor);

private:

   boost::filesystem::path path_;
   bool remote_;
};

class RemoteResourcePath : public ResourcePath {
public:

   RemoteResourcePath( const boost::filesystem::path& path ) :
      ResourcePath( path, true ) {};

};

class LocalResourcePath : public ResourcePath {
public:

   LocalResourcePath( const boost::filesystem::path& path ) :
      ResourcePath( path, false ) {};

};

} // namespace rsync
} // namespace liber

#endif // RESOURCE_PATH_H
