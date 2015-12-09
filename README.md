# EXASOL R SDK
Copyright © EXASOL AG, Nuremberg (Germany). All rights reserved.  
2004 - 2015

[![Build Status](https://travis-ci.org/vsimko/r-exasol.svg)](https://travis-ci.org/vsimko/r-exasol)
[![codecov.io](https://codecov.io/github/vsimko/r-exasol/coverage.svg?branch=master)](https://codecov.io/github/vsimko/r-exasol?branch=master)

## Installation

You need to have **[Rtools](https://cran.r-project.org/bin/windows/Rtools/)** installed on Windows for R to be able to compile the package (for Linux see below).
Plus, the R package **devtools** must be available in your workspace as it contains the `install_github()` method. The EXASOL package itself depends on the R package **RODBC**, which should be available on install from Github.

To install the EXASOL package:
```r
require(devtools)
install_github("EXASOL/r-exasol")
```

# EXASOL R Package

## Description

The EXASOL R Package offers functionality to interact with
EXASOL database out of R programs. It is developed as a wrapper
around RODBC and extends RODBC in two main aspects:

1. It offers fast data transfer between EXASOL and R, multiple
   times faster than RODBC.  This is achieved by using a proprietary
   transfer channel which is optimized for batch loading. Please read
   the R help of `exa.readData()` and `exa.writeData()` for details or
   read below.

2. It makes it convenient to run parts of your R code in parallel on EXASOL DB, using EXASOL R UDF scripts behind
   the scenes. For example you can define an R function and execute it
   in parallel on different groups of data in an EXASOL DB
   table. Please read the R help of `exa.createScript()` function for
   details or read the example below.


## Prerequisites and Installation

1. Install developer extensions for R to be able to build from sources

   For Windows: Install Rtools
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

3. Install a recent version of the RODBC package.

4. To build and install the package from sources manually:
 - go to folder `UDF/R`
 - Run `R CMD INSTALL .` from the command line (sudo if you are on linux)
   to build and install the package.


## Importing the package

To use the package import it as follows:
``` r
library(exasol)
```

## Connecting to EXASOL DB

First, you have to create a ODBC connection via RODBC that will be
used by all functions of this package.

We recommend to create a DSN pointing to your EXASOL database
instance.  Read the README of EXASOL's ODBC driver package for
details. Assuming you have a DSN pointing to your database instance
you can connect like this:

``` r
C <- odbcConnect("yourDSN")
```

Alternatively if you don't have a DSN you can also specify the
required information in the connection string:

``` r
C <- odbcDriverConnect("Driver=/path/to/libexaodbc-uo2214lv1.so;UID=sys;PWD=exasol;EXAHOST=exasolution-host:8563")
```

You can also read the RODBC documentation via `help(odbcConnect)` for
more details.


## Using the EXASOL R Package

We offer the following three methods that operate on the RODBC
connection:

 1. Execute query on EXASOL DB and transfer results to R,
    using fast batch transfer:
``` r
exa.readData(connection, query)
```

 2. Write data frame from R to EXASOL DB, using fast batch transfer:
``` r
exa.writeData(connection, dataFrameToWrite, table = 'targetTable')
```

 3. Create a script:
``` r
exa.createScript(cnx, nameForTheScript, inArgs, outArgs, rFunctionToExecute)
```

The detailed documentation for the package and all methods is
available directly in R via:
``` r
help(exa.exasol)
help(exa.readData)
help(exa.writeData)
help(exa.createScript)
```

The help also explains the optional parameters that are available for
some of the functions.

## Example Program
```r
library(RODBC)
library(exasol)

# Connect via RODBC
C <- odbcConnect("DSNToYourEXASOL")

# Read results 
tables <- exa.readData(C, "SELECT * FROM EXA_ALL_TABLES")

# Work with the data frame returned (examples)
print(nrow(tables))          # print number of rows
print(colnames(tables))      # print names of columns
print(tables[1,])            # print first row
print(tables$TABLE_NAME[1])  # print first value of specified column

# Generate example data frame with two groups 
# of random values with different means.
valsMean0  <- rnorm(10, 0)
valsMean50 <- rnorm(10, 50)
twogroups <- data.frame(group = rep(1:2, each = 10),
                        value = c(valsMean0, valsMean50))

# Write example data to a table
odbcQuery(C, "CREATE SCHEMA test")
odbcQuery(C, "CREATE TABLE test.twogroups (groupid INT, val DOUBLE)")
exa.writeData(C, twogroups, tableName = "test.twogroups")

# Create the R function as an UDF R script in the database
# In our case it computes the mean for each group.
testscript <- exa.createScript(
   C,
   "test.mymean",
   function(data) {
       data$next_row(NA);  # read all values from this group into a single vector.
       data$emit(data$groupid[[1]], mean(data$val))
   },
   inArgs = c( "groupid INT", "val DOUBLE" ),
   outArgs = c( "groupid INT", "mean DOUBLE" ) )

# Run the function, grouping by the groupid column
# and aggregating on the "val" column. This returns
# two values which are close to the means of the two groups.
testscript ("groupid", "val", table = "test.twogroups" , groupBy = "groupid")
```

## Show output from EXASOL UDF scripts

During the development and debugging of UDF scripts it is helpful to be able to
output arbitrary information from the UDF to any console. For this purpose we
offer a small Python output service `exaoutput.py`, which is included in the
*[EXASOL Python Package](https://www.exasol.com/portal/display/WEL/Home)*.
A recent version of Python 2.7 is required on the client system.

To use the output service simply start it like follows:
```sh
$> python exaoutput.py
```

You can also specify a customized port:
```sh
$> python exaoutput.py -p 4555
```

The service prints the address it listens to when it is started:
```sh
$> python exaoutput.py
>>> bind the output server to 192.168.5.61:3000
```

This address should be given to the exa.createScript function:

```r
mySum <- exa.createScript(cnx, mySum,
                       inArgs = c("a INT", "b INT"),
                       outArgs = c("c INT"),
                       outputAddress = c("192.168.5.61", 3000),
                       func = function(data) {
   print("@@@ fetch data")
   data$next_row(NA)
   a <- data$a
   b <- data$b
   print(paste("@@@ calculate sum on", length(a), "rows"))
   data$emit(a + b)
   print("@@@ calculation done")
 })
```

On call of the script, the printed output will appear in the terminal
where the output service is started:  
```
>>> bind the output server to 192.168.5.61:3000  
192.168.6.132:59282> [1] "@@@ fetch data"  
192.168.6.132:59282> [1] "@@@ calculate sum on 5000 rows"  
192.168.6.132:59282> [1] "@@@ calculation done"  
```

All output line are prefixed with source IP and and source port, what
means, that each R instance on EXASOL DB have unique output prefix.
