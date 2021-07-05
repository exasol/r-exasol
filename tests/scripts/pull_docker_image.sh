#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined r-version/cran repo as argument"
    exit 1
fi

COMBINED_R_VERSION="$1"
SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_complete_name.sh" "${COMBINED_R_VERSION}")"

docker pull "$IMAGE_NAME" || true
