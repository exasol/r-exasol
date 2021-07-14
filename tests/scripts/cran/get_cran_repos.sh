#!/bin/bash

set -euo pipefail

#hard coded list of cran repositories, from which we pick all major releases
supported_cran_repos=("bionic-cran40" "bionic-cran35")
echo "${supported_cran_repos[@]}"
