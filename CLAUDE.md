# CLAUDE.md

## Build & Test

- **Build:** `R CMD INSTALL --no-multiarch --with-keep.source -l ~/R/library .`
- **Test:** `HAS_LOCAL_EXASOL_TEST_DB=true EXAHOST=localhost:8888 EXAUID=sys EXAPWD=exasol Rscript -e 'devtools::test()'`
- Exasol docker-db: `EXAHOST=localhost:8888`, user `sys`, password `exasol`.
- **DBI/DBItest prerequisite:** the DBItest suite (`tests/testthat/test-DBItest.R`) requires the legacy forks `marcelboldt/DBI` and `marcelboldt/DBItest` — CRAN `DBI` >= 0.4 dropped `dbiCheckCompliance`, which `DBItest 1.0.1` still imports, so test discovery fails with *"object 'dbiCheckCompliance' is not exported by 'namespace:DBI'"*. Install once with `Rscript -e 'remotes::install_github("marcelboldt/DBI"); remotes::install_github("marcelboldt/DBItest")'` — note this downgrades system-wide `DBI`, so prefer a project-local library (`R_LIBS_USER=~/R/r-exasol-libs ...`) if you also work on other R projects.

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

