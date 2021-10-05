# R Interface & SDK for the EXASOL Database

[![Build Status](https://github.com/exasol/r-exasol/actions/workflows/main.yml/badge.svg)](https://github.com/exasol/r-exasol)
[![codecov.io](https://codecov.io/github/exasol/r-exasol/coverage.svg?branch=main)](https://codecov.io/github/exasol/r-exasol?branch=main)

###### Please note that this is an open source project which is officially supported by Exasol. For any question, you can contact our support team.

## Description


The Exasol R Package offers interface functionality such as connecting to, querying and writing
into an Exasol Database (version 5 onwards). It is optimized for fast reading &
writing from and to a multinode cluster. Implemented are DBI compliant methods for database access,
querying and modification. The package integrates with Exasol's in-DB R UDF capabilities, which
allows to deploy and execute R code dynamically from an R application running on a client.

Exasol is an in-memory RDBMS that runs in a MPP cluster (shared-nothing) environment.
Leading the TPC-H benchmark, it is considered the fastest analytical data warehouse available.

*The community edition can be downloaded for free from the [Exasol Community Portal](https://docs.exasol.com/get_started/communityedition.htm).*

## Status

The package gets tested in Github against R 3.5, 3.6, 4.0 and 4.1, but it also worked on R 3.3 and R 3.4 in the past. For R 3.6 and earlier, an older RODBC package is necessary, the newer versions (1.3-17 and upwards) only support R 4.0. You can use the [remotes package](https://remotes.r-lib.org/) to install specific versions of RODBC. The packages DBI 0.3.1 and a fork of DBItest 1.0-1 are not the current versions. The Exasol R package may however work with DBI 0.4.1, but until this is resolved the fitting older packages can be loaded from here:

- https://github.com/marcelboldt/DBI
- https://github.com/marcelboldt/DBItest

The low-level methods such as regards `exa.readData`, `exa.writeData` and `exa.createScript` may work as expected, so
should the DBI connection methods (`dbConnect`, etc.; all tests passed). DBI querying methods also get close to being production ready.

Following test were implemented:
* Unit tests for the C++ high speed data transfer layer
* Integration test (which require a local Exasol db running, and therefore are not activated by default). These leverage the DBItest package.

`dplyr` methods have been moved to a [separate package](https://github.com/marcelboldt/r-exasol-dplyr).

Code coverage of tests:
[![codecov.io](https://codecov.io/github/exasol/r-exasol/coverage.svg?branch=main)](https://codecov.io/github/exasol/r-exasol?branch=main)

### Known issues

The latest ODBC driver (7.1.1) under MacOsX BigSur has a dependency issue. If you have problems under MacOsX and see an error message like ```...libexaodbc-io418sys.dylib not found```, please revert to the previous ODBC driver (7.0.11) which is know to work. 

## Getting started

### Prerequisites

1. Install developer extensions for R to be able to build from sources

   For Windows: Install `Rtools` matching your version of R from [here](https://cran.r-project.org/bin/windows/Rtools/).
   
   For Linux: Install the `R-devel` (RPM) or `r-base-dev` (Debian) package.
   
   For MacOsX: If you install R via Homebrew, use the CASK version: ```brew install --cask r```. (The normal version, ```brew install r``` will only install packages via source, which will require additional dependencies)


   The packages might change over time, so generally you can go to the
   official R-project website download section for your OS to find the
   package that are needed to build packages.

2. Make sure you have ODBC and Exasol ODBC installed and
   configured on your system. Go to the [download page](https://docs.exasol.com/connect_exasol/drivers/odbc.htm) and select your Exasol version in the left menu. Then scroll down to 'Download ODBC Driver' and choose the appropriate version.
   
   Once installed, we recommend to create a DSN pointing to
   your database instance. Read the README of Exasol's ODBC
   driver package for details.

   <span style="color:red">*Important:*</span> Since v6.1.0 the Exasol ODBC driver for Linux and MacOsX are not bundled anymore in r-exasol and you need to 
   configure properly the ODBC driver on your system. 
   If you do not indicate a DSN calling dbConnect, r-exasol will look for a Data Source named <em>"EXASolution Driver"</em> and 
   you must have the respective driver configured properly, for example the following line in your ```/etc/odbcinst.ini```:
   ```
   [EXASolution Driver]
   Driver=/usr/lib/libexaodbc-uo2214lv2.so
   ```
   (This change does not affect Windows: Under Windows the ODBC driver already had to be configured on previous versions.) 

   #### Linux
   On Linux, you also need to install the development files for ODBC.
   Therefore please install `unixodbc-devel` (RPM) or `unixodbc-dev`
   (Debian) package.

   #### MacOsX
   On MacOsX you can install `unixodbc` with Homebrew:
   ```brew install unixodbc```

3. The R package **devtools** must be available as it contains the `install_github()` method
   and the things needed to build the package.

   You can install it in R with: ```install.packages("devtools")```.
   If you followed step 1., this installation should work on Linux/Windows.
   Under MacOsX you might need to install additional dependencies, please check the [devtools page](https://www.rdocumentation.org/packages/devtools/versions/1.13.6)


4. OpenSSL
   For Windows: The package downloads the required DLL's during the installation process.

   For Linux: Install using the common package manager, for example: ```apt install libssl-dev```

   For MacOsX: Install via brew: ```brew install openssl```.

5. Install the necessary dependencies, such as RODBC or the DBI packages. For the versions of these package, checkout the section [Status](#status). Have a look into the [Github Actions Docker](https://github.com/exasol/r-exasol/blob/main/tests/Dockerfile), if you look for an example, how to install the necessary dependencies.

   
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
# OR connect to Exasol DB running on default port (8563) with a hostname, default 'sys' user and default schema ('SYS'), using an encrypted channel
con <- dbConnect("exa", exahost = "<hostname>:8563", uid = "sys", pwd = "<password>", encrypted = "Y")

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
