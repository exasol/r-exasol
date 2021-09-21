#!/bin/bash

set -euo pipefail


R CMD check --output=/log --as-cran .