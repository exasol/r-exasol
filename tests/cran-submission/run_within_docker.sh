#!/bin/bash

set -euo pipefail

export PATH=$PATH:/scripts

R CMD check --output=/log --as-cran .