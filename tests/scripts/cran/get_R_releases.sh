#!/bin/bash

set -e

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

R_RELEASES=$("$SCRIPT_DIR/parse_R_releases.sh")

#Use jq to transform the array into JSON format.
#'jq -R ." reads raw arrays and 'jq -cs' compacts and encapsulates all items in one array.
printf '%s\n' "${R_RELEASES[@]}" | jq -R . | jq -cs .