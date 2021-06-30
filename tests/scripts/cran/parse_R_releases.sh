#!/bin/bash

set -e

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

all_available_versions=$(apt-cache madison r-base-core)
relevant_repos=$($SCRIPT_DIR/get_cran_repos.sh)
pat=$(echo $relevant_repos|tr " " "|")
relevant_versions=$(echo "$all_available_versions" | grep -E "$pat")


minor_versions=($(echo "$relevant_versions" | cut -f 2 -d "|" | cut -f 1,2 -d "."))
declare -A minor_versions_for_images
for minor_version in "${minor_versions[@]}"
do
  newest_minor_version=$(echo "$relevant_versions" | cut -f 2 -d "|" | sed 's/^ *//;s/ *$//' | grep -E "^$minor_version" | head -n 1)
  minor_versions_for_images[$minor_version]=$newest_minor_version
done

echo "${minor_versions_for_images[@]}"