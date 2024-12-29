
# CaloX Docker Images


```
docker build -t calox --build-arg git_user_name=<git user name> --build-arg git_user_email=<email> .
```

```
docker run -it -v /path/to/code:/sim -v /path/to/build:/work calox
source /opt/geant4/install/share/Geant4-11.0.2/geant4make/geant4make.sh
cd /work
cmake /sim -B g4_sim_build
...
```


## CaloX HGcal image

```
docker build -t calox-hgcal -f CaloX_G4.Dockerfile
```
The HGcal image builds the CaloX HGcal G4 simulation found in the ``dev`` branch of the `CaloX_G4 <https://github.com/cowden/CaloX_G4>`_
git repository.

