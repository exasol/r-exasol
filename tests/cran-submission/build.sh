#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined cran repo/r-version as arguments, for example: bionic-cran40/4.1.1-1.1804.0"
    exit 1
fi

CRAN_REPO=$(echo "$1" | cut -f 1 -d "/")
R_VERSION=$(echo "$1" | cut -f 2 -d "/")
SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

docker build -t "r_exasol_for_cran" --build-arg R_VERSION="$R_VERSION" --build-arg CRAN_REPO="$CRAN_REPO" "$SCRIPT_DIR"
