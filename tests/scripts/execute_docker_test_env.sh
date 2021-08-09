#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined r-version/cran repo as argument"
    exit 1
fi

VARIANT=
if [ $# -gt 1 ]; then
  VARIANT=$2
fi

COMBINED_R_VERSION=$1

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
"$SCRIPT_DIR"/build_docker_test_env.sh "$COMBINED_R_VERSION"
docker container rm -f -v r_test || true
IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_complete_name.sh" "${COMBINED_R_VERSION}")"
PROJ_DIR="$(realpath "$SCRIPT_DIR/../..")"
docker run  --network host --name r_test -v "$PROJ_DIR":"$PROJ_DIR" -t "$IMAGE_NAME" "$PROJ_DIR" $VARIANT