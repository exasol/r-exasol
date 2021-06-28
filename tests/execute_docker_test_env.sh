#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
docker build -t r:test_env "$SCRIPT_DIR"
docker run  --network host --name r_test -v "$SCRIPT_DIR/../":/test -t r:test_env