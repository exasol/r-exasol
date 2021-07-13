#!/bin/bash

set -euo pipefail

#Goal:
#Get all minor versions for interested cran repositories in the format: $CranRepo/Release
#For example: "bionic-cran35/3.5.3-1bionic bionic-cran35/3.6.3-1bionic bionic-cran40/4.0.5-1.1804.0 bionic-cran40/4.1.0-1.1804.0"

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

#get all available versions of r-base-core on the system.
#assumes that the cran repositoriers were already installed (see install_cran_repos)
#output is for example:
#r-base-core | 4.1.0-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
#r-base-core | 4.0.5-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
#r-base-core | 4.0.4-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
#r-base-core | 4.0.3-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
#r-base-core | 4.0.2-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
#r-base-core | 4.0.1-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
#r-base-core | 4.0.0-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
all_available_versions=$(apt-cache madison r-base-core)

relevant_repos=($("$SCRIPT_DIR"/get_cran_repos.sh))

#iterate over all cran repositories which we are interested in. ignore repositories which come from default linux distribution
for rel_repo in "${relevant_repos[@]}"
  do
  #filter only those version which are included in the cran repositories we are interested in
  pat=$(echo "$rel_repo"|tr " " "|")
  relevant_versions=$(echo "$all_available_versions" | grep -E "$pat")


  minor_versions=($(echo "$relevant_versions" | cut -f 2 -d "|" | cut -f 1,2 -d "."))
  declare -A minor_versions_for_images
  #iterate over minor versions in current cran repo
  for minor_version in "${minor_versions[@]}"
    do
      #Take the first line of all releases matching the current minor release, for example:
      ##r-base-core | 4.0.5-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
      ##r-base-core | 4.0.4-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
      ##r-base-core | 4.0.3-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
      ##r-base-core | 4.0.2-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
      ##r-base-core | 4.0.1-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
      ##r-base-core | 4.0.0-1.1804.0 | https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/ Packages
      # => Take the first line, second column, trimmed (using sed)
      #Note that this assumes that madison lists the releases always in decreasing order
      newest_minor_version=$(echo "$relevant_versions" | cut -f 2 -d "|" | sed 's/^ *//;s/ *$//' | grep -E "^$minor_version" | head -n 1)
      minor_versions_for_images[$minor_version]="$rel_repo/$newest_minor_version"
    done

  done
echo "${minor_versions_for_images[@]}"