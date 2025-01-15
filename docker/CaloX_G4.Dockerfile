FROM calox

SHELL ["/usr/bin/bash", "-c"]
RUN /opt/miniconda/bin/conda init && \
    source ~/.bashrc && \
    conda install python=3.11

ADD dkr_build_calox_g4.sh /opt/dkr_build_calox_g4.sh

RUN bash /opt/dkr_build_calox_g4.sh 


