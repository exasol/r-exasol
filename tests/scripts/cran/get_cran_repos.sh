#!/bin/bash

set -euo pipefail

#hard coded list of cran repositories, from which we pick all major releases
supported_cran_repos=("noble-cran40")
echo "${supported_cran_repos[@]}"
