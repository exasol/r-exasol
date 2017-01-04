# R Interface & SDK for the EXASOL Database

[![Build Status](https://travis-ci.org/EXASOL/r-exasol.svg)](https://travis-ci.org/EXASOL/r-exasol)
[![codecov.io](https://codecov.io/github/EXASOL/r-exasol/coverage.svg?branch=master)](https://codecov.io/github/EXASOL/r-exasol?branch=master)
[![Issue Stats](http://issuestats.com/github/EXASOL/r-exasol/badge/pr?style=flat-square)](http://issuestats.com/github/EXASOL/r-exasol)
[![Issue Stats](http://issuestats.com/github/EXASOL/r-exasol/badge/issue?style=flat-square)](http://issuestats.com/github/EXASOL/r-exasol)

###### Please note that this is an open source project which is officially supported by EXASOL. For any question, you can contact our support team.

## Installation

You need to have [Rtools](https://cran.r-project.org/bin/windows/Rtools/)
installed on Windows for R to be able to compile the package (for Linux see below).
Plus, the R package **devtools** must be available as it contains the `install_github()` method. The EXASOL package itself
depends on the R package **RODBC**, which should be available on install
from Github.

To install the EXASOL package (after having installed the pre-requisites described further down):
```r
devtools::install_github("EXASOL/r-exasol")
```

## Status

The package is currently tested with R 3.3.1. The packages however (DBI 0.3.1 and a fork of DBItest 1.0-1) are not the current versions.
The EXASOL R package may however work with DBI 0.4.1, but until this is resolved the fitting older packages can be loaded from here:

- https://github.com/marcelboldt/DBI
- https://github.com/marcelboldt/DBItest

The low-level methods as regards exa.readData, exa.writeData and exa.createScript may work as expected, so
should the DBI connection methods (dbConnect etc.; all tests passed). DBI querying methods also get close to being production ready.

Writing tests have not yet been implemented (only quick-tests), but it hopefully happens in the near future.

`dplyr` methods have been moved to a [separate package](https://github.com/marcelboldt/r-exasol-dplyr).

Code coverage of tests:

![codecov.io](https://codecov.io/github/EXASOL/r-exasol/branch.svg?branch=master)

# EXASOL R Package

## Example

```r
devtools::install_github("exasol/r-exasol")
library(exasol)

# display package documentation with examples for each method
?exasol
# display documentation of individual commands with Exasol-specific parameters
?dbConnect

# connect to EXASOL DB with an ODBC DSN
con <- dbConnect("exa", dsn="ExaSolo", schema="TEST")
# OR connect to EXASOL DB running on default port (8563) with a hostname, default 'sys' user and default schema ('SYS')
# (e.g. Exasol on AWS)
con <- dbConnect("exa", exahost = "<hostname>:8563", uid = "sys", pwd = "<password>")


# list all tables in EXASOL (returns a character vector).
dbListTables(con)


# send a query and read the result into a data.frame
df <- dbGetQuery(con, "select * from TEST.MYTAB")


#send a query and return a result set handler, then fetch 2 rows
res <- dbSendQuery(con, "select * from TEST.MYTAB")
df <- dbFetch(res, 2)


# disconnect
dbDisconnect(con)
```


## Description

The EXASOL R Package offers interface functionality such as connecting to, querying and writing
into an EXASOL Database (version 5 onwards). It is optimised for massively parallel reading &
writing from and to a multinode cluster. Implemented are DBI compliant methods for database access,
querying and modiifcation. The package integrates with EXASOL's InDB R UDF capabilities, which
allows to deploy and execute R code dynamically from an R application running on a client.

EXASOL is an InMemory RDBMS that runs in a MPP cluster (shared-nothing) environment.
Leading the TPC-H benckmark, it is considered the fastest analytical data warehouse available.
The community edition can be downloaded for free from the [EXASOL Community Portal](https://www.exasol.com/portal).


## Prerequisites and Installation

1. Install developer extensions for R to be able to build from sources

   For Windows: Install `Rtools`
   For Linux: Install the `R-devel` (RPM) or `r-base-dev` (Debian) package.

   The packages might change over time, so generally you can go to the
   official R-project website download section for your OS to find the
   package that are needed to build packages.

2. Make sure you have ODBC and EXASOL ODBC installed and
   configured on your system. We recommend to create a DSN pointing to
   your database instance. Read the README of EXASOL's ODBC
   driver package for details.

   On Linux, you also need to install the development files for ODBC.
   Therefore please install `unixodbc-devel` (RPM) or `unixodbc-dev`
   (Debian) package.

## Importing the package

To use the package import it as follows:
``` r
library(exasol)
```

## Manual

The package comes with documentation accessible from R via the command `?exasol`.

## Package index

### DBI: Driver-related

EXADriver-class
:	An interface driver object to the EXASOL Database.

dbDriver
:	Load database drivers.

dbUnloadDriver
:	Unload a driver.

dbConnect
:	Creates a connection to an EXASOL Database.

dbListConnections
:	List currently open connections.

### DBI: Connection-related

EXAConnection-class
:	An object holding a connection to an EXASOL Database.

dbDisconnect
:	Disconnects the connection.

dbSendQuery
:	Sends an SQL statment to an EXASOL DB, prepares for result fetching.

dbGetQuery
:	Executes the query, fetches and returns the entire result set.

dbGetException
:	TODO

dbListResults
:	TODO

dbListFields
:	TODO

dbListTables
:	TODO

dbReadTable
:	Reads a DB table.

dbWriteTable
:	Writes a data.frame into a table. If the table does not exist, it is created.

dbExistsTable
:	Checks if a table exists in an EXASOL DB.

dbRemoveTable
:	Removes a table.

dbBegin
:	Starts a DB transaction.

dbEnd
:	Ends a DB transaction.

dbCommit
:	Sends a commit.

dbRollback
:	Rolls the current DB transaction back.

### DBI: Result set-related

EXAResult-class
:	An object that is associated with a result set in an EXASOL Database.

dbFetch
:	Fetches a subset of an result set.

dbClearResult
:	Frees all resources associated with an EXAResult.

dbColumnInfo
:	TODO

dbGetStatement
:	TODO

dbHasCompleted
:	TODO

dbGetRowsAffected
:	TODO

dbGetRowCount
:	TODO

### Low-level methods

exa.readData
:	Execute a SQL query on an EXASolution database and read results fast.

exa.writeData
:	Write a data.frame into an EXASOL table fast.

exa.createScript
:	Deploys an R function as an UDF in the EXASolution database.

EXAupper
:	Changes an identifier into uppercase, except for it is quoted.

dbCurrentSchema
:	Fetches and outputs the current schema from an EXASOL DB.
