# r-exasol 0.1.0, released t.b.d.

Code name: t.b.d.

## Summary 

t.b.d.

## Features / Enhancements


* 1st version
* Fixed compilation problem on Linux systems (e.g. Linux Mint 17). The problem was the missing "-lodbc" flag which caused the error "undefined symbol: SQLFreeHandle"
* Merge pull request #2 from vsimko/master
* Added config file for Travis-CI
* travis config copied from biwavelet package
* Improved tests for createScript function - better code coverage
* Cleaned the lambda function that is returned from `exa.createScript`. The code now checks types of input arguments and constructs the SQL statement in a cleaner way.
* Added startup message that shows the package version when the `exasol` package is loaded.
* Added first test to `exa.readData`
* Added a test that tries to connect to a local EXASOL VM. This test is ignored on Travis.
* added `test-writeData.R`
* Changed type of EMITS, SET, RETURNS and SCALAR constants, they are now strings. We can now use `match.arg()` to check for allowed in/out types easily.
* Merge pull request #4 from vsimko/master
* introd. DBI compatibility
* Merge branch 'master' of https://github.com/marcelboldt/r-exasol
* exaodbc 3rd
* Update .travis.yml
* exaodbc 4th
* exaodbc 5th
* Merge pull request #6 from marcelboldt/master
* Merge remote-tracking branch 'origin/master'
* passed DBItest getting_started, driver, connection
* Merge pull request #7 from vsimko/master
* corrected DSN
* Merge pull request #8 from marcelboldt/master
* Merge pull request #10 from marcelboldt/master
* Added some tests to skip-list because they caused errors when running CHECK locally. These tests were automatically skipped when building on travis anyways.
* added an additional test "dbCurrentSchema" just to check whether code coverage can be improved
* Merge pull request #11 from vsimko/master
* Merge pull request #1 from EXASOL/master
* Merge branch 'master' of https://github.com/marcelboldt/r-exasol
* Merge pull request #12 from marcelboldt/master
* Merge pull request #14 from marcelboldt/master
* added "issuestats" badge
* added "issuestats" badge
* Merge remote-tracking branch 'origin/master'
* faster travis builds
* Update test-DBItest.R
* Merge pull request #18 from marcelboldt/master
* Merge pull request #16 from vsimko/master
* Update .travis.yml
* added dplyr support
* Merge branch 'master' of https://github.com/marcelboldt/r-exasol
* update travis.yml
* added param to constructor
* removed DBItest "constructor"
* Merge branch 'master' of https://github.com/marcelboldt/r-exasol into marcelboldt-master
* Merge branch 'marcelboldt-master'
* added driver parameter to EXANewConnection
* ODBC driver for MacOS & Linux included, changed error msg in IOstart
* added is.null() condition to exasol()
* Merge pull request #20 from marcelboldt/master
* Merge pull request #23 from marcelboldt/master
* R-3.3.0 necessary (clearing temp results)
* Merge pull request #24 from marcelboldt/master
* Merge pull request #26 from ianformanek/master
* changed db schema for travis PUB1092
* merge commit with upstream
* Clarify reference to prerequisites
* Merge pull request #27 from ianformanek/master
* Merge remote-tracking branch 'upstream/master'
* Merge branch 'master' of https://github.com/marcelboldt/r-exasol
* Merge pull request #28 from marcelboldt/master
* Merge pull request #29 from marcelboldt/master
* Merge pull request #30 from vsimko/master
* Add matrix build to travis with oldrel and release of R (#43)
* Add dbHasCompleted for EXAResult (#49)
* #41 : added connection pane snippets (#57)

## Bug Fixes
* Fixed problem with `ifelse` vs `if...else`. The problem is that the result type of `ifelse` matches the type of the first parameter.
* fixed the link to Travis-CI
* Trying to fix a problem with lintr. Lintr reports the following error: `no visible global function definition for "assert_that"`
* Using `ifelse` is actually in this case is OK because both TRUE and FALSE params are scalar.
* fixed a typo "travis-i" to "travis-ci"
* Fixed bugs in exa.readData: add. params, numerals
* correction to allow opening cons with dbConnect(drv="exasol",...)
* fixes, travis odbc 1st
* travis odbc 2nd
* lots of fixes regarding querying
* typo
* fixed many CMD CHECK warnings
* minor reformatting
* fixed encoding issues
* fixed issues related to quoted identifiers
* fixed a bug in dbSendQuery, ExaExecStmt. Now the con standard schema is used, if not defined in the stmt
* excluded test_result
* several bugfixes
* fixing lintr warnings "no visible global function definition"
* fixes: e.g. read_booleans,...
* resolved conflict rest-DBItest.R
* corrected drv paths
* fixed in_addr conversion
* mac receives header... cleanup debugging msgs
* all to 1
* ifndef _WIN32 y=1
* APPLE & ELSE to from 1 to 0
* corrected values for __APPLE__
* add error () for ndef WIN32
* add error() for __APPLE__
* APPLE: 0
* APPLE: 0x01010000
* ifdef __APPLE__
* memset proxy_header -> 0
* decl & init errno
* errno on POSIX sys
* ntohs
* ntohl
* changed darwin ODBC driver
* Revert "changed darwin ODBC driver"
* EXAResult tbls are now cleared up more reliably
* fixed issue #22 - thanks abudis
* moved dplyr methods to exasol-dplyr package & fixed bug: datatype def varchar NA
* not suggests dplyr
* bugfix dbReadTable schema handling
* Fix typo in connection error hint
* fixed issuestats badges, added example
* bugfix dbReadTable schema handling
* Fixed compilation warning
* Fix Issue 37: Allow comments in the beginning of Queries (#39)
* Fix start-test-env call in .travis.yaml (#44)
* Fix CI config (#46)
* Fix schema filter for dbListTables (#51)
* #40: Fixed empty char column data type mapping (#55)
* Removed broken badges

## Documentation

* Update README.md
* Update README.md
* Update README.md
* Update README.md
* moved exa.writeData to a separate file and added documentation as a doc-comment that is parsed by roxygen.
* moved exa.readData to a separate file and added documentation as a doc-comment that is parsed by roxygen.
* moved exa.createScript to a separate file and added documentation as a doc-comment that is parsed by roxygen.
* enabled automated generation of NAMESPACE using roxygen
* In order to pass R CMD CHECK, I added dummy comments for the following C functions:  - `C_asyncRODBCQueryStart`  - `C_asyncRODBCIOStart`  - `C_asyncRODBCIsDone`  - `C_asyncRODBCMax`  - `C_asyncRODBCProxyHost`  - `C_asyncRODBCProxyPort`  - `C_asyncRODBCQueryCheck`  - `C_asyncRODBCQueryFinish`
* Added `\dontrun` for the examples to pass R CMD CHECK
* Regenerated documentation using newer roxygen version.
* Fixed some formatting in `README.md`
* Update README.md
* Update README.md
* Update README.md
* Update README.md
* Update README.md
* Update README.md
* Update README.md
* Update README.md
* Update README.md
* Added project status
* Update README.md
* Update README.md
* Couple improvements in README to help getting started
* Remove mention of AWS
* update readme installation section
* update readme installation section
* readme changes
* readme changes
* readme changes
* using new roxygen version
* README clean-up
* Update README.md
* Update supported R Versions in README (#45)

## Refactoring

* Changesthat will later allow unit-testing
* minor coding style fixes
* Moved the example from `exa.createScript.R` to a separate `examples/createScript.R` file
* Added `mockOnly` parameter to `exa.createScript` in order to support unit-testing. I also increased R package version number.
* Reformatted C code in `exasol.c` for better understandability.
* Added apt dependency `unixodbc-dev` that provides `sql.h`
* unixodbc-dev used in travis config file
* added `.travis.yml` to `.Rbuildignore` to pass R CMD CHECK
* enabled "dontrun" within the createScript example
* Added 2x badges : Travis-CI + Codecov
* Moved examples from `readData` and `writeData` to separate files in `examples/` directory
* changed DBItest repo to marcelboldt/DBItest
* removed /lib folder

## Dependency updates

### Compile Dependency Updates
* `DBI`
* `RODBC`
* `methods`
* `devtools`

#### Linux
* `unixodbc-dev`
* `R-devel` (RPM) or `r-base-dev` (Debian) package.

####Windows
* `Rtools`

### Runtime Dependency Updates
* Exasol ODBC driver

### Test Dependency Updates
* dbiTest
* assertthat

