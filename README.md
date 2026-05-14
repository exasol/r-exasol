# R Interface & SDK for the EXASOL Database

[![Build Status](https://github.com/exasol/r-exasol/actions/workflows/main.yml/badge.svg)](https://github.com/exasol/r-exasol)

###### Please note that this is an open source project which is officially supported by Exasol. For any question, you can contact our support team.

## Description


The Exasol R Package offers interface functionality such as connecting to, querying and writing
into an Exasol Database (version 7.1 onwards) via Exasol's native WebSocket protocol. No ODBC
driver installation is required; the package communicates directly with the database using
JSON-over-WebSocket for command operations and HTTP/HTTPS for high-speed bulk data transfer.
It is optimized for fast reading & writing from and to a multinode cluster. Implemented are
DBI compliant methods for database access, querying and modification. The package integrates
with Exasol's in-DB R UDF capabilities, which allows to deploy and execute R code dynamically
from an R application running on a client.

Exasol is an in-memory RDBMS that runs in a MPP cluster (shared-nothing) environment.
Leading the TPC-H benchmark, it is considered the fastest analytical data warehouse available.

*The community edition can be downloaded for free from the [Exasol Community Portal](https://docs.exasol.com/get_started/communityedition.htm).*

## Status

Github CI build validates the package r-exasol against R versions 4.0, 4.1, 4.2, and 4.3. Please note that DBI 0.3.1 and a fork of DBItest 1.0-1 are not the latest versions. The Exasol R package may however work with DBI 0.4.1, but until this is resolved appropriate and compatible older packages must be installed from here:

- https://github.com/marcelboldt/DBI
- https://github.com/marcelboldt/DBItest

Install both with:

```r
remotes::install_github("marcelboldt/DBI")
remotes::install_github("marcelboldt/DBItest")
```

Without these, the DBItest-based integration tests (`tests/testthat/test-DBItest.R`) fail to load with the error *"object 'dbiCheckCompliance' is not exported by 'namespace:DBI'"*, because CRAN `DBI` >= 0.4 dropped that symbol while `DBItest 1.0.1` still imports it. Note that installing the fork downgrades your system-wide `DBI` — if you also work on other R projects, install into a project-local library by setting `R_LIBS_USER` before the install commands.

For Windows only:
  * As this package uses C++17 code, it needs at least RTools >= 4.0.0. Hence, it works only on R >= 4.0.0.
  * Please note that the version of RTools must match the installed version of R (RTools 4.2 for R >= 4.2, RTools 4.0 for R4.0/4.1), check https://cran.r-project.org/bin/windows/Rtools/ for further information

The low-level methods such as regards `exa.readData`, `exa.writeData` and `exa.createScript` may work as expected, so
should the DBI connection methods (`dbConnect`, etc.; all tests passed). DBI querying methods also get close to being production ready.

Following test were implemented:
* Unit tests for the C++ high speed data transfer layer
* Integration test (which require a local Exasol db running, and therefore are not activated by default). These leverage the DBItest package.

`dplyr` methods have been moved to a [separate package](https://github.com/marcelboldt/r-exasol-dplyr).

## Getting started

### Prerequisites

1. Install developer extensions for R to be able to build from sources

   For Windows: Install `Rtools` matching your version of R from [here](https://cran.r-project.org/bin/windows/Rtools/), but at least version 4.0.0.
   
   For Linux: Install the `R-base-devel` (RPM) or `r-base-dev` (Debian) package.
   
   For MacOsX: If you install R via Homebrew, use the CASK version: ```brew install --cask r```. (The normal version, ```brew install r``` will only install packages via source, which will require additional dependencies)


   The packages might change over time, so generally you can go to the
   official R-project website download section for your OS to find the
   package that are needed to build packages.

2. The R package **devtools** must be available as it contains the `install_github()` method
   and the things needed to build the package.

   You can install it in R with: ```install.packages("devtools")```.
   If you followed step 1., this installation should work on Linux/Windows.
   Under MacOsX you might need to install additional dependencies, please check the [devtools page](https://www.rdocumentation.org/packages/devtools/versions/1.13.6)


3. OpenSSL (required for the encrypted WebSocket connection and HTTPS bulk transfer)
   For Windows: The package downloads the required DLL's during the installation process.

   For Linux: Install using the common package manager, for example: ```apt install libssl-dev```

   For MacOsX: Install via brew: ```brew install openssl```.

4. Install the necessary R dependencies. `DBI` (and, for running the integration test suite, `DBItest`) must be installed from the **legacy forks** described in the section [Status](#status):

   ```r
   remotes::install_github("marcelboldt/DBI")
   remotes::install_github("marcelboldt/DBItest")
   ```

   The current CRAN `DBI` (>= 0.4) is incompatible with the `DBItest 1.0.1` fork that this package targets. See the [Github Actions Dockerfile](https://github.com/exasol/r-exasol/blob/main/tests/Dockerfile) for a complete example of the dependency install sequence.

   
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

# connect to Exasol DB on default port (8563) with hostname, 'sys' user and 'SYS' schema, using an encrypted (TLS) WebSocket
con <- dbConnect("exa", exahost = "<hostname>:8563", uid = "sys", pwd = "<password>", encryption = "Y")

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
