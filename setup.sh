#!/bin/sh
# Download data and extract it.
wget -c http://www.iai.uni-bonn.de/~behley/data/sdi2015-data.zip
unzip -qo sdi2015-data.zip
rm sdi2015-data.zip

# build project files and run test...
mkdir build
cd build 
cmake ..
make
cd ..
./runtests

