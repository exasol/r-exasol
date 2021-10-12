#!/bin/bash

set -euo pipefail

export PATH=$PATH:/scripts

R CMD check --output=/log --as-cran /work/exasol_7.0.1.tar.gz