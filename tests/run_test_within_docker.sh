#!/bin/bash

if [ -z "${1-}" ]
then
  TEST_DIR="/test"
else
  TEST_DIR="$1"
fi

cd "${TEST_DIR}"
Rscript -e 'devtools::install()'
cd tests
R -f testthat.R