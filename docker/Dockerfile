# Docker file to build the TTU-CaloX Geant4 Simulation and Analysis Image
FROM centos as base

ENV LANG C.UTF-8

RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
RUN sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*

RUN yum install -y git vim-enhanced which wget curl gcc gdb \
    cmake

# install python - anaconda
RUN mkdir -p /opt/loaders
RUN wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -P /opt/loaders && \ 
    /bin/sh /opt/loaders/Miniconda3-latest-Linux-x86_64.sh -b -p /opt/miniconda && \
    /opt/miniconda/bin/conda init

##############################
# Development Environment
#############################
RUN git clone https://github.com/VundleVim/Vundle.vim.git /root/.vim/bundle/Vundle.vim && \
    vim +PluginInstall +qall && \
    git clone https://github.com/cowden/CowTools /root/CowTools && \
    cp /root/CowTools/config/vimrc /root/.vimrc && \
    git config --global user.name "USERNAME" && \
    git config --global user.email "USEREMAIL"

# install ROOT
RUN yum install -y epel-release && \
    yum install -y root

############################
# GEANT4
###########################
RUN mkdir -p /opt/geant4/build && \
    mkdir -p /opt/geant4/data && \
    mkdir -p /opt/geant4/src && \
    mkdir -p /opt/geant4/install
# optional dependencies
# CLHEP, Expat, zlib, Xerces C++ headers
RUN yum install -y expat expat-devel
# download and unpack G4
RUN wget https://github.com/Geant4/geant4/archive/refs/tags/v11.0.2.tar.gz && \
    tar -xzf v11.0.2.tar.gz -C /opt/geant4/src/
# configure the build
RUN cd /opt/geant4/build/ && \
    cmake -DCMAKE_INSTALL_PREFIX=/opt/geant4/install \
          -DGEANT4_INSTALL_DATA=ON \
          -DGEANT4_INSTALL_DATADIR=/opt/geant4/data \
          -DGEANT4_BUILD_MULTITHREAD=ON \
          -DBUILD_SHARED_LIBS=ON \
          -DBUILD_STATIC_LIBS=ON \
          /opt/geant4/src/geant4-11.0.2
RUN cd /opt/geant4/build/ && \
    make -j`nproc` && \
    make install
