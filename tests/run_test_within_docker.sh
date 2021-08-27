#!/bin/bash

set -euo pipefail

if [ $# -lt 1 ]; then
    echo "You must provide project directory as argument"
    exit 1
fi

TEST_DIR="$1"

#Install r-exasol library via devtools
cd "${TEST_DIR}"
Rscript -e 'devtools::install()'

export HAS_LOCAL_EXASOL_TEST_DB=true

TST_CMD="R -f testthat.R"


if [ $# -gt 1 ]; then
  case "$2" in
    valgrind)
      export DEBIAN_FRONTEND=noninteractive
      apt install -y valgrind
      TST_CMD="R -d valgrind -f testthat.R"
      ;;
    asan)
      export LD_PRELOAD=/usr/lib/gcc/x86_64-linux-gnu/7/libasan.so
      ;;
    asan_no_leak)
      export ASAN_OPTIONS=detect_leaks=0 #disable leak detection because R itself has memory leaks. It would break
      export LD_PRELOAD=/usr/lib/gcc/x86_64-linux-gnu/7/libasan.so
    ;;
  esac
fi

#run unit tests
cd tests
$TST_CMD