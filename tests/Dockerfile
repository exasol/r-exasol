FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y dirmngr software-properties-common && \
    gpg --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys E298A3A825C0D65DFD57CBB651716619E084DAB9 && \
    gpg -a --export E298A3A825C0D65DFD57CBB651716619E084DAB9 | apt-key add -  && \
    add-apt-repository -y 'deb https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/'

ARG DEBIAN_FRONTEND=noninteractive

RUN add-apt-repository -y ppa:deadsnakes/ppa
RUN apt-get update && \
    apt-get install -y r-base-core r-base-dev curl libcurl4-openssl-dev libssl-dev \
                        libssh2-1-dev libxml2-dev zlib1g-dev unixodbc-dev
RUN Rscript -e 'install.packages("testthat")'
RUN  Rscript -e 'install.packages("devtools", dependencies = TRUE)'
RUN  Rscript -e 'install.packages("remotes")'
RUN  Rscript -e 'library(remotes);install_version("RODBC","1.3-16")'
RUN  Rscript -e 'devtools::install_github("jimhester/covr")'
RUN  Rscript -e 'devtools::install_github("jimhester/lintr")'
RUN  Rscript -e 'devtools::install_github("marcelboldt/DBI")'
RUN  Rscript -e 'devtools::install_github("marcelboldt/DBItest")'

COPY run_test_within_docker.sh /run_test_within_docker.sh

ENTRYPOINT ["/run_test_within_docker.sh"]