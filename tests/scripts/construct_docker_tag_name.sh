#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined r-version/cran repo as argument"
    exit 1
fi

CRAN_REPO=$(echo "$1" | cut -f 1 -d "/")
R_VERSION=$(echo "$1" | cut -f 2 -d "/")

echo "test-container-latest-$CRAN_REPO-r$R_VERSION"
