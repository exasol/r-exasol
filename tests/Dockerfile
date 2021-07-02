FROM ubuntu:18.04


COPY scripts/cran /scripts

RUN bash -c "/scripts/install_cran_repos.sh"

ARG DEBIAN_FRONTEND=noninteractive
ARG R_VERSION

RUN apt-get -y update && \
    apt-get -y install --no-install-recommends locales && \
    locale-gen en_US.UTF-8 && \
    update-locale LC_ALL=en_US.UTF-8 && \
    apt-get -y clean

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN add-apt-repository -y ppa:deadsnakes/ppa
RUN apt-get update && \
    apt-get install -y --no-install-recommends "r-base-core=$R_VERSION" "r-base-dev=$R_VERSION" curl \
                        libcurl4-openssl-dev libssl-dev \
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