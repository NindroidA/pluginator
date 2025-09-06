#!/bin/bash

# build the makeshite
echo "Building Pluginator ..."
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..

# make sure ownership is set correctly
echo "Setting ownership ..."
sudo chown -R $(whoami):amp .

# set build perms
echo "Setting permissions for build directory ..."
chmod 755 build/pluginator

# make sure perms for data directories are guud
echo "Confirming permissions for data files and directories ..."
chmod 664 data/*.json
chmod 664 config/pluginator.config
chmod 664 lang/*.json
chmod -R 775 config/ data/ lang/

# bada bing bada done
echo "Done!"