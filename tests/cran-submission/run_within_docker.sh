#!/bin/bash

set -euo pipefail

export PATH=$PATH:/scripts

R CMD check --output=/log --as-cran /work/exasol_6.0.0.tar.gz