#!/bin/bash

set -e

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
#$SCRIPT_DIR/install_cran_repos.sh

R_RELEASES=$("$SCRIPT_DIR/parse_R_releases.sh")
printf '%s\n' "${R_RELEASES[@]}" | jq -R . | jq -cs .