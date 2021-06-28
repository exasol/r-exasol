#!/bin/bash

cd /test
Rscript -e 'devtools::install()'
cd tests
R -f testthat.R