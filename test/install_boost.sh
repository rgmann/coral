#!/bin/bash

# Abort on failed simple commands.
# set -ex

urlize_boost_version()
{
   echo $1 | tr . _
}

B2_FLAGS="-a -j2 threading=multi --build-type=minimal --layout=tagged"

BOOST_VERSION=1.59.0
BOOST_INSTALL_PREFIX=/usr/local
if [[ $(uname) == 'Darwin' ]]; then
   BOOST_INSTALL_PREFIX=/opt/local
fi

BOOST_INSTALL_LIBS=

BOOST_URL_BASE=http://sourceforge.net/projects/boost/files/boost
DOWNLOADS_DIR=`pwd`/third_party
BOOST_VERSION_URL="boost_$(urlize_boost_version $BOOST_VERSION)"
BOOST_PACKAGE="$BOOST_VERSION_URL.tar.bz2"


download_boost()
{
   if [ ! -d $DOWNLOADS_DIR ]; then
      echo "Cannot download boost. Destination directory does not exist."
      exit 1
   fi

   local download_product=$DOWNLOADS_DIR/${BOOST_PACKAGE}
   if [ -e $download_product ]; then
      echo "Boost ${BOOST_VERSION} already downloaded. Skipping download step."
   else
      local boost_url=$BOOST_URL_BASE/${BOOST_VERSION}/${BOOST_PACKAGE}/download
      echo "Downloading boost version ${BOOST_VERSION} from ${boost_url}"

      curl -L -o $download_product $boost_url

      if [ $? -ne 0 ]; then
         echo "Failed to download boost."
         exit 1
      fi
   fi
}

unpack_boost()
{
   if [ -d $DOWNLOADS_DIR ]; then
      cd $DOWNLOADS_DIR
      if [ -e $BOOST_PACKAGE ]; then
         echo "Unpacking $BOOST_PACKAGE."
         tar xfj $BOOST_PACKAGE
      else
         echo "Expected packet $BOOST_PACKAGE does not exist."
         exit 1
      fi
      cd -
   else
      echo "Download directory does not exist at $DOWNLOADS_DIR"
      exit 1
   fi
}

build_boost()
{
   local install_prefix=$1
   local boost_unpack_path=$DOWNLOADS_DIR/$BOOST_VERSION_URL

   if [ -d $boost_unpack_path ]; then
      local start_path=$(pwd)
      cd $boost_unpack_path

      if [ -e ./bootstrap.sh ]; then
         ./bootstrap.sh --prefix=$BOOST_INSTALL_PREFIX
         if [ $? -eq 0 ]; then

            ./b2 $B2_FLAGS $BOOST_INSTALL_LIBS install
            if [ $? -ne 0 ]; then
               echo "Boost installation failed."
               exit 1
            else
               echo "Boost installation complete."
            fi
         else
            echo "Boost bootstrap.sh script failed."
            exit 1
         fi
      else
         echo "Boost bootstrap.sh script does not exist."
         exit 1
      fi

      cd $start_path
   else
      echo "Failed to find unpacked boost package at $boost_unpack_path."
   fi
}

missing_parameter()
{
   echo $1 requires a parameter
   exit 1
}

unknown_parameter()
{
   if [[ -n $2 &&  $2 != "" ]]; then
      echo Unknown argument \"$2\" for parameter $1.
   else
      echo Unknown argument $1
   fi
   exit 1
}

parse_args()
{
   while [ "$1" != "" ]; do
      case $1 in
         -h | -\?)
            usage
            exit
            ;;
         --boost-version)
            if [ -n $2 ]; then
               BOOST_VERSION=$2
               shift
            else
               missing_parameter $1
            fi
            ;;
         --prefix)
            if [ -n $2 ]; then
               BOOST_INSTALL_PREFIX=$2
               shift
            else
               missing_parameter $1
            fi
            ;;
         --libs)
            if [ -n $2 ]; then
               IFS=',' read -ra LIBS <<< "$2"
               for lib in "${LIBS[@]}"; do
                  BOOST_INSTALL_LIBS="$BOOST_INSTALL_LIBS --with-$lib"
               done
               shift
            else
               missing_parameter $1
            fi
            ;;
         *)
            unknown_parameter $1
            ;;
      esac
      shift
   done
}

# 
# unpack_boost
parse_args $@
echo $BOOST_INSTALL_LIBS
echo $BOOST_INSTALL_PREFIX

download_boost
unpack_boost
build_boost

