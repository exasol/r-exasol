#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide combined r-version/cran repo as argument"
    exit 1
fi

COMBINED_R_VERSION=$1

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_name.sh")"
TAG_NAME="$("$SCRIPT_DIR"/construct_docker_tag_name.sh "$COMBINED_R_VERSION")"


AVAILABLE_ON_DOCKER_HUB="$("$SCRIPT_DIR/list_tags.sh" "$IMAGE_NAME" "$TAG_NAME")"

echo "Available on dockerhub: $AVAILABLE_ON_DOCKER_HUB"
if [[ -z $AVAILABLE_ON_DOCKER_HUB ]]; then
  "$SCRIPT_DIR"/push_docker_image.sh "$R_VERSION"
fi
