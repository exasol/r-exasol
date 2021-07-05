#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined r-version/cran repo as argument"
    exit 1
fi

COMBINED_R_VERSION="$1"
R_VERSION=$(echo "$1" | cut -f 1 -d "/")
CRAN_REPO=$(echo "$1" | cut -f 2 -d "/")

echo "test-container-latest-r$R_VERSION-$CRAN_REPO"
