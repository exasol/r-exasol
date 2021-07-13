#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide project directory as argument"
    exit 1
fi

TEST_DIR="$1"

#Install r-exasol library via devtools
cd "${TEST_DIR}"
Rscript -e 'devtools::install()'

#run unit tests
cd tests
R -f testthat.R