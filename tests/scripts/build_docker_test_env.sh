#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined r-version/cran repo as arguments"
    exit 1
fi

COMBINED_R_VERSION=$1
CRAN_REPO=$(echo "$1" | cut -f 1 -d "/")
R_VERSION=$(echo "$1" | cut -f 2 -d "/")


SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_complete_name.sh" "${COMBINED_R_VERSION}")"
docker build -t "$IMAGE_NAME" --cache-from "$IMAGE_NAME" --build-arg GH_TOKEN="$GITHUB_TOKEN" --build-arg R_VERSION="$R_VERSION" --build-arg CRAN_REPO="$CRAN_REPO" "$SCRIPT_DIR/../"
