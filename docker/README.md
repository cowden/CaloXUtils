
# CaloX Docker Images

```
sed -i "s/USERNAME/Your Name/" Dockerfile
sed -i "s/USEREMAIL/your.email@serv.domain/" Dockerfile
```

```
docker build -t calox .
```

```
docker run -it -v /path/to/code:/sim -v /path/to/build:/work calox
source /opt/geant4/install/share/Geant4-11.0.2/geant4make/geant4make.sh
cd /work
cmake /sim -B g4_sim_build
...
```

