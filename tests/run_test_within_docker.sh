#!/bin/bash


if [ $# -lt 1 ]; then
    echo "You must provide project directory as argument"
    exit 1
fi

TEST_DIR="$1"

cd "${TEST_DIR}"
Rscript -e 'devtools::install()'
cd tests
R -f testthat.R