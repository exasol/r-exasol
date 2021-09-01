# r-exasol 6.0.0, released 2021-09-01

Code name: Encryption support.

## Summary 

This release contains a major refactoring of the C++ layer. This also includes the removal of several C functions, which were exposed to the R-layer, but effectively unused. Also, r-exasol now supports encryption of the data channel via TLS.

## Features / Enhancements
 #63 Refactor C layer
 #56:Created SSL Socket

## Bug Fixes
 #71 Fixed docker image nightly upload
 #78: Fixed cpp and windows/mac build on GHA (#80)
 #73: Added debug log

### Compile Dependency Updates
 #OpenSSL

#### Linux
n/a

####Windows
n/a

### Runtime Dependency Updates
n/a

### Test Dependency Updates
n/a