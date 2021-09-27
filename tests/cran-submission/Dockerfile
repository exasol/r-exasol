FROM ubuntu:18.04


ARG DEBIAN_FRONTEND=noninteractive
ARG R_VERSION
ARG CRAN_REPO

RUN apt-get update && \
    apt-get install -y dirmngr software-properties-common && \
    gpg --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys E298A3A825C0D65DFD57CBB651716619E084DAB9 && \
    gpg -a --export E298A3A825C0D65DFD57CBB651716619E084DAB9 | apt-key add -

RUN add-apt-repository -y "deb https://cloud.r-project.org/bin/linux/ubuntu $CRAN_REPO/"

RUN apt-get -y update && \
    apt-get -y install --no-install-recommends locales && \
    locale-gen en_US.UTF-8 && \
    update-locale LC_ALL=en_US.UTF-8 && \
    apt-get -y clean && \
    apt-get -y autoremove

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN add-apt-repository -y ppa:deadsnakes/ppa
RUN apt-get update && \
    apt-get install -y --no-install-recommends "r-base-core=$R_VERSION" "r-base-dev=$R_VERSION" curl \
                        libcurl4-openssl-dev libssl-dev \
                        libssh2-1-dev libxml2-dev zlib1g-dev unixodbc-dev \
                        texlive-latex-base texlive-fonts-recommended texlive-fonts-extra \
                        texlive-latex-extra texinfo && \
                        apt-get -y clean && apt-get -y autoremove
RUN Rscript -e 'install.packages("testthat")'
RUN Rscript -e 'install.packages("DBI")'
RUN Rscript -e 'install.packages("assertthat")'
RUN Rscript -e 'install.packages("xml2")'
RUN Rscript -e 'install.packages("DBItest")'
RUN Rscript -e 'install.packages("devtools", dependencies=TRUE)'
RUN Rscript -e 'install.packages("RODBC")'

RUN mkdir -p /scripts && \
    curl -L -o /scripts/checkbashisms https://sourceforge.net/projects/checkbaskisms/files/2.0.0.2/checkbashisms/download && \
    chmod +x /scripts/checkbashisms

COPY run_within_docker.sh /run_within_docker.sh
RUN mkdir -p /log
