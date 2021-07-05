#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide r-version as argument"
    exit 1
fi

R_VERSION="$1"

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_name.sh")"
TAG_NAME="$("$SCRIPT_DIR"/construct_docker_tag_name.sh "$R_VERSION")"


AVAILABLE_ON_DOCKER_HUB="$("$SCRIPT_DIR/list_tags.sh" "$IMAGE_NAME" "$TAG_NAME")"

if [[ -z $AVAILABLE_ON_DOCKER_HUB ]]; then
  "$SCRIPT_DIR"/push_docker_image.sh "$R_VERSION"
fi
