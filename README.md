# R Interface & SDK for the EXASOL Database

[![Build Status](https://travis-ci.org/EXASOL/r-exasol.svg)](https://travis-ci.org/EXASOL/r-exasol)
[![codecov.io](https://codecov.io/github/EXASOL/r-exasol/coverage.svg?branch=master)](https://codecov.io/github/EXASOL/r-exasol?branch=master)
[![Issue Stats](http://issuestats.com/github/EXASOL/r-exasol/badge/pr?style=flat-square)](http://issuestats.com/github/EXASOL/r-exasol)
[![Issue Stats](http://issuestats.com/github/EXASOL/r-exasol/badge/issue?style=flat-square)](http://issuestats.com/github/EXASOL/r-exasol)

###### Please note that this is an open source project which is officially supported by Exasol. For any question, you can contact our support team.

## Description


The Exasol R Package offers interface functionality such as connecting to, querying and writing
into an Exasol Database (version 5 onwards). It is optimised for fast reading &
writing from and to a multinode cluster. Implemented are DBI compliant methods for database access,
querying and modification. The package integrates with Exasol's in-DB R UDF capabilities, which
allows to deploy and execute R code dynamically from an R application running on a client.

Exasol is an in-memory RDBMS that runs in a MPP cluster (shared-nothing) environment.
Leading the TPC-H benckmark, it is considered the fastest analytical data warehouse available.

*The community edition can be downloaded for free from the [Exasol Community Portal](https://www.exasol.com/portal).*

## Status

The package is currently tested with R 3.6 and R 4.0, but it is also worked on R 3.3 and R 3.4 The packages however (DBI 0.3.1 and a fork of DBItest 1.0-1) are not the current versions.
The Exasol R package may however work with DBI 0.4.1, but until this is resolved the fitting older packages can be loaded from here:

- https://github.com/marcelboldt/DBI
- https://github.com/marcelboldt/DBItest

The low-level methods sucb as regards `exa.readData`, `exa.writeData` and `exa.createScript` may work as expected, so
should the DBI connection methods (`dbConnect`, etc.; all tests passed). DBI querying methods also get close to being production ready.

Writing tests have not yet been implemented (only quick-tests), but it hopefully happens in the near future.

`dplyr` methods have been moved to a [separate package](https://github.com/marcelboldt/r-exasol-dplyr).

Code coverage of tests:

![codecov.io](https://codecov.io/github/EXASOL/r-exasol/branch.svg?branch=master)


## Getting started

### Prerequisites

1. Install developer extensions for R to be able to build from sources

   For Windows: Install `Rtools` matching your version of R from [here](https://cran.r-project.org/bin/windows/Rtools/).
   
   For Linux: Install the `R-devel` (RPM) or `r-base-dev` (Debian) package.

   The packages might change over time, so generally you can go to the
   official R-project website download section for your OS to find the
   package that are needed to build packages.

2. Make sure you have ODBC and Exasol ODBC installed and
   configured on your system. Go to the [download page](https://www.exasol.com/portal/display/DOWNLOAD/) and select your Exasol version in the left menu. Then scroll down to 'Download ODBC Driver' and choose the appropriate version.
   
   Once installed, we recommend to create a DSN pointing to
   your database instance. Read the README of Exasol's ODBC
   driver package for details.

   On Linux, you also need to install the development files for ODBC.
   Therefore please install `unixodbc-devel` (RPM) or `unixodbc-dev`
   (Debian) package.
   
3. The R package **devtools** must be available as it contains the `install_github()` method
   and the things needed to build the package.
   
### Installation

To load the Exasol package from GitHub and have R compile and build the package for use:
```r
devtools::install_github("EXASOL/r-exasol")
```   

### Loading the package

To use the package import it as follows:
```r
library(exasol)
```


### Use

```r
devtools::install_github("exasol/r-exasol")
library(exasol)

# display package documentation with examples for each method
?exasol
# display documentation of individual commands with Exasol-specific parameters
?dbConnect

# connect to Exasol DB with an ODBC DSN
con <- dbConnect("exa", dsn="ExaSolo", schema="test")
# OR connect to Exasol DB running on default port (8563) with a hostname, default 'sys' user and default schema ('SYS')
con <- dbConnect("exa", exahost = "<hostname>:8563", uid = "sys", pwd = "<password>")

# list all tables in Exasol (returns a character vector).
dbListTables(con)

# send a query and read the result into a data.frame
df <- dbGetQuery(con, "SELECT * FROM test.mytab")

#send a query and return a result set handler, then fetch 2 rows
res <- dbSendQuery(con, "SELECT * FROM test.mytab")
df <- dbFetch(res, 2)

# disconnect
dbDisconnect(con)
```

## Documentation

The package comes with documentation accessible from R via the command `?exasol`.
