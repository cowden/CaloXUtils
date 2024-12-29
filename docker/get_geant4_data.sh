#!/usr/bin/env bash

BASE_LOC=https://cern.ch/geant4-data/datasets

for dataset in G4NDL.4.7.1.tar.gz G4EMLOW.8.5.tar.gz G4PhotonEvaporation.5.7.tar.gz G4RadioactiveDecay.5.6.tar.gz G4PARTICLEXS.4.0.tar.gz G4PII.1.3.tar.gz G4RealSurface.2.2.tar.gz G4SAIDDATA.2.0.tar.gz G4ABLA.3.3.tar.gz G4INCL.1.2.tar.gz G4ENSDFSTATE.2.3.tar.gz G4TENDL.1.4.tar.gz; do
    wget $BASE_LOC/$dataset
done


