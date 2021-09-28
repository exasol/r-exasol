# r-exasol 6.1.0, released t.b.d.

Code name: t.b.d.

## Summary 

This release contains a major refactoring of the R layer.
Following functions has been dropped:
* "dbApply" has been dropped, as it was not implemented and currently there are no plans to pursue this approach.
* "dbListConnections" has been dropped as it is marked DEPRECATED in DBI package.

Important: The bundled ODBC driver for Linux/Mac was removed (as CRAN requires to remove any binary executables from the package); dbConnect("exa",...) now will look in some default folders for the driver, please check the [README](../../README.md) for more details.

## Features / Enhancements

## Bug Fixes

### Compile Dependency Updates

#### Linux
n/a

####Windows
n/a

### Runtime Dependency Updates
n/a

### Test Dependency Updates
n/a