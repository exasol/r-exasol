#!/bin/bash

set -e

if [ $# -lt 1 ]; then
    echo "You must provide r-version as argument"
    exit 1
fi

R_VERSION="$1"

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
$SCRIPT_DIR/build_docker_test_env.sh $R_VERSION
docker container rm -f -v r_test || true
IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_complete_name.sh" "${R_VERSION}")"
docker run  --network host --name r_test -v "$SCRIPT_DIR/../..":/test -t $IMAGE_NAME