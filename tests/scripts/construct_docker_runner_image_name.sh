#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide r-version as argument"
    exit 1
fi

R_VERSION="$1"

echo "exasol/r-exasol:test-container-latest-r$R_VERSION"
