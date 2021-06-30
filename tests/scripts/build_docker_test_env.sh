#!/bin/bash

set -e

if [ $# -lt 1 ]; then
    echo "You must provide r-version as argument"
    exit 1
fi

R_VERSION="$1"

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_runner_image_name.sh" ""${R_VERSION}")"
docker build -t "$IMAGE_NAME" --cache-from "$IMAGE_NAME" --build-arg R_VERSION=$R_VERSION "../"
