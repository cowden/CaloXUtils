# Docker file to build the TTU-CaloX Geant4 Simulation and Analysis Image
FROM quay.io/centos/centos:stream8 as base

ARG git_user_name
ARG git_user_email

ENV LANG C.UTF-8

RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
RUN sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*

RUN yum update -y libmodulemd && \
    yum install -y git vim-enhanced which wget curl gcc gdb \
    cmake

# install python - anaconda
RUN mkdir -p /opt/loaders
RUN wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -P /opt/loaders && \ 
    /bin/bash /opt/loaders/Miniconda3-latest-Linux-x86_64.sh -b -p /opt/miniconda && \
    /opt/miniconda/bin/conda init bash && \
    /opt/miniconda/bin/conda install -n base -y python=3.11 pip

##############################
# Development Environment
#############################
RUN git clone https://github.com/VundleVim/Vundle.vim.git /root/.vim/bundle/Vundle.vim && \
    vim +PluginInstall +qall && \
    git clone https://github.com/cowden/CowTools /root/CowTools && \
    cp /root/CowTools/config/vimrc /root/.vimrc && \
    git config --global user.name "${git_user_name}" && \
    git config --global user.email "${git_user_email}"

# install ROOT
RUN yum install -y epel-release && \
    dnf config-manager --set-enabled powertools && \
    yum install -y root
    #yum install -y hdf5-mpich hdf5-mpich-devel && \
    #ln -s /usr/lib64/gfortran/modules/mpich/hdf5.mod /usr/lib64/gfortran/modules/hdf5.mod && \
    #ln -s /usr/include/mpich-x86_64/hdf5.h /usr/include/hdf5.h

##############################
# BUILD HDF5
##############################
RUN wget https://github.com/HDFGroup/hdf5/releases/download/hdf5_1.14.5/hdf5-1.14.5.tar.gz && \
    mkdir -p /opt/hdf5 && \
    tar -xzf hdf5-1.14.5.tar.gz -C /opt/hdf5 && \
    cd /opt/hdf5/hdf5-1.14.5/ && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    make && \
    make install


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
RUN wget https://github.com/Geant4/geant4/archive/refs/tags/v11.2.2.tar.gz && \
    tar -xzf v11.2.2.tar.gz -C /opt/geant4/src/
# configure the build
RUN cd /opt/geant4/build/ && \
    cmake -DCMAKE_INSTALL_PREFIX=/opt/geant4/install \
          #-DGEANT4_INSTALL_DATA=ON \
          #-DGEANT4_INSTALL_DATA_TIMEOUT=15000 \
          -DGEANT4_INSTALL_DATADIR=/opt/geant4/data \
          -DGEANT4_BUILD_MULTITHREADED=ON \
          -DGEANT4_USE_ROOT=ON \
          #-DGEANT4_USE_HDF5=ON \
          -DBUILD_SHARED_LIBS=ON \
          -DBUILD_STATIC_LIBS=ON \
          /opt/geant4/src/geant4-11.2.2
ADD *.tar.gz /opt/geant4/data/
RUN cd /opt/geant4/build/ && \
    make  -j`nproc` && \
    make install

RUN echo -e "\n\nsource /opt/geant4/install/share/Geant4/geant4make/geant4make.sh" >> ${HOME}/.bashrc  && \
    echo -e "\n\nexport PATH=${PATH}:/usr/local/HDF_Group/HDF5/1.14.5/bin" >> ${HOME}/.bashrc
