# CLAUDE.md

## Build & Test

- **Build:** `R CMD INSTALL --no-multiarch --with-keep.source -l ~/R/library .`
- **Test:** `EXAHOST=localhost:8888 EXAUID=sys EXAPWD=exasol Rscript -e 'devtools::test()'`
- Exasol docker-db: `EXAHOST=localhost:8888`, user `sys`, password `exasol`.

## Code Review

When reviewing C++ code, run clang-tidy on changed `.cpp` files and include findings in the review. Only run on files that were changed, not the entire codebase.

```bash
RCPP_INC=$(Rscript -e "cat(system.file(\"include\", package=\"Rcpp\"))" 2>/dev/null)
BH_INC=$(Rscript -e "cat(system.file(\"include\", package=\"BH\"))" 2>/dev/null)
TT_INC=$(Rscript -e "cat(system.file(\"include\", package=\"testthat\"))" 2>/dev/null)
cd /home/tu/Work/r-exasol/src && clang-tidy \
  -checks="-*,bugprone-*,performance-*,modernize-*,readability-*,clang-analyzer-*" \
  --extra-arg="-std=c++17" \
  --extra-arg="-I." \
  --extra-arg="-I./r_exasol/external" \
  --extra-arg="-I${RCPP_INC}" \
  --extra-arg="-I${BH_INC}" \
  --extra-arg="-I${TT_INC}" \
  --extra-arg="-I/usr/share/R/include" \
  <file.cpp> 2>&1

```

