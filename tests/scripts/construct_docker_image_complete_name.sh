#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide r-version as argument"
    exit 1
fi

R_VERSION="$1"

IMAGE_NAME="$("$SCRIPT_DIR/construct_docker_image_name.sh")"
TAG_NAME="$("$SCRIPT_DIR/construct_docker_tag_name.sh" ${R_VERSION})"
echo "$IMAGE_NAME:$TAG_NAME"
