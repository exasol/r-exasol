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
CERTIFICATE_PATH=${R_TESTS_CERTIFICATE_PATH:-/tmp/certificate/} #Get path where certificate of the docker-db is stored. We then mount this path under "/certificate" in the test container, which is the location the CI tests expect it to be placed.
docker run  --network host --name r_test -v "$PROJ_DIR":"$PROJ_DIR" -v "$CERTIFICATE_PATH":"/certificate/" --add-host exasol-test-database:127.0.0.1 -t "$IMAGE_NAME" "$PROJ_DIR" "$VARIANT"
