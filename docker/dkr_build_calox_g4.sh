#!/bin/bash

source /opt/geant4/install/share/Geant4/geant4make/geant4make.sh
echo "Cloning CaloX_G4"
git clone https://github.com/cowden/CaloX_G4 --branch=dev -- /opt/CaloX_G4

echo "Building CaloX_G4"    
cd /opt/CaloX_G4
mkdir build
cd build
cmake ../
make
echo "CaloX_G4 build complete: /opt/CaloX_G4/build/"
