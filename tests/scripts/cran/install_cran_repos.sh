#!/bin/bash

set -e

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
REPOS=$("$SCRIPT_DIR/get_cran_repos.sh")

apt-get update && \
    apt-get install -y dirmngr software-properties-common && \
    gpg --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys E298A3A825C0D65DFD57CBB651716619E084DAB9 && \
    gpg -a --export E298A3A825C0D65DFD57CBB651716619E084DAB9 | apt-key add -

for repo in $REPOS; \
    do \
    add-apt-repository -y "deb https://cloud.r-project.org/bin/linux/ubuntu "$repo"/"; \
    done
