# r-exasol 6.1.0, released t.b.d.

Code name: t.b.d.

## Summary 

This release contains a major refactoring of the R layer.
The function "dbApply" has been dropped, as it was not implemented and currently there are no plans to pursue this approach.
Important: The bundled ODBC driver for Linux/Mac was removed (as CRAN requires to remove any binary executables from the package); dbConnect("exa",...) now will look in some default folders for the driver, please check documentation for "exa".

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