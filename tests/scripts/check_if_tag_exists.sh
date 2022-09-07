#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]
then
cat << HELP

check_if_tag_exists.sh  --  validates if tag exists for a Docker image on a remote registry.
                            Returns an empty string if tag does not exist, otherwise the tag name on remote repository.

HELP
fi

image="$1"
tag="$2"
tags=`wget -q https://registry.hub.docker.com/v2/repositories/${image}/tags/$tag -O -| jq .name || true`

echo "${tags}"
