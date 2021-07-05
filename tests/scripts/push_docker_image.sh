#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide r-version as argument"
    exit 1
fi

R_VERSION="$1"

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_name.sh")"

"$SCRIPT_DIR"/build_docker_test_env.sh "$R_VERSION"
docker push "$IMAGE_NAME"
