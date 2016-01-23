#!/bin/bash
set -ex

PROTOBUF_INSTALL_PREFIX=/usr/local
if [[ $(uname) == 'Darwin' ]]; then
   PROTOBUF_INSTALL_PREFIX=/opt/local
fi

VERSION=2.6.1
PACKAGE_NAME=protobuf-$VERSION
PACKAGE_ARCHIVE_NAME=$PACKAGE_NAME.tar.gz

CURRENT_DIR=$(pwd)
cd third_party/
#wget https://protobuf.googlecode.com/files/$PACKAGE_ARCHIVE_NAME
wget https://github.com/google/protobuf/releases/download/v$VERSION/$PACKAGE_ARCHIVE_NAME
if [ $? -eq 0 ]; then
   tar -xzvf $PACKAGE_ARCHIVE_NAME
   if [ -d $PACKAGE_NAME ]; then
      echo "Installing protobuf to $PROTOBUF_INSTALL_PREFIX"
      cd $PACKAGE_NAME && ./configure --prefix=$PROTOBUF_INSTALL_PREFIX && make && sudo make install
   else
      echo "Failed to extract $PACKAGE_NAME"
   fi
else
   echo "Failed to download $PACKAGE_NAME"
fi
cd $CURRENT_DIR

