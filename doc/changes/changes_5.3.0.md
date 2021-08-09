# r-exasol 5.3.0, released 2021-08-09

Code name: Initial official release

## Summary 

This is the first official release of r-exasol. 

## Features / Enhancements
* Initial implementation
* #41: Added connection pane snippets (#57)
* #61: Added standard documentation (#62)
* #59: Migrated CI build system from Travis to Github actions (#60)
* #65: Added logging errno when socket access fails (#66)

## Bug Fixes
* #40: Fixed empty char column data type mapping (#55)

### Compile Dependency Updates
* Added `DBI (>= 0.3.1)`
* Added `RODBC (>= 1.3-12)`
* Added `methods`
* Added `devtools`

#### Linux
* Added `unixodbc-dev`
* Added `R-devel` (RPM) or `r-base-dev` (Debian) package.

####Windows
* Added `Rtools`

### Runtime Dependency Updates
* Added Exasol ODBC driver

### Test Dependency Updates
* Added dbiTest
* Added assertthat
