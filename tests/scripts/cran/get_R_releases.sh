#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

R_RELEASES=$("$SCRIPT_DIR/parse_R_releases.sh")

#Use jq to transform the array into JSON format.
#'jq -R ." reads raw arrays and 'jq -cs' compacts and encapsulates all items in one array.
#result is something like: ["bionic-cran35/3.5.3-1bionic","bionic-cran35/3.6.3-1bionic","bionic-cran40/4.0.5-1.1804.0","bionic-cran40/4.1.0-1.1804.0"]
printf '%s\n' ${R_RELEASES[@]} | jq -R . | jq -cs .