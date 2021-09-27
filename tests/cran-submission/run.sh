#!/bin/bash

set -euo pipefail



SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
R_EXASOL_BASE_DIR="$(realpath "$SCRIPT_DIR/../../..")"


docker run --name r_exasol_for_cran -v $R_EXASOL_BASE_DIR:/work --tmpfs /log:exec --rm -i -t "r_exasol_for_cran" bash
