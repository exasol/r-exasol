# r-exasol Developer Guide

r-exasol is a R library providing access to an Exasol DB. 
This document is about the inner working of r-exasol.



## Problem Statement:
As described in the [README.md](../../README.md):
> It is optimised for fast reading & writing from and to a multinode cluster.
> 
This document will describe how the r-exasol library reads/write data from/to the database from high level perspective.
The focus is hereby on the functions which leverage the fast data transfer, as the other functions make usage of standard DBI / RODBC functions.

## Implementation

### Connection

When the client calls dbConnect(), the library connects to the Exasol DB via RODBC, this happens in `EXADBI.R`:
```r
    EXANewConnection <- function(...)
    ...
    odbcDriverConnect(...)
```

### Data Exchange

Once the connection is ready, the client can read data or manipulate data using basically two functions:

```r
    exa.readData()
    exa.writeData()
```

Some of the standard SQL functions use internally one of those two methods, for example `dbGetQuery()`,
but that is not always the case.

### Internal data flow when reading from database

The following diagram shows the internal data flow when the client invokes ```exa.readData()```:

![](./images/r-exasol.png)

When the client invokes ```readData()``` the library reads the server info (hostname/port) from the ODBC connection and establishes a TCP socket (A1). When the server receives a connection request on the listening port it creates another TCP port (A2) which will be used for data transfer. As last step within ```asyncRODBCIOStart()``` the client reads the information of the new port (A3), which it will need to prepare the export SQL statements. 
Then the library calls```asyncRODBCQueryStart()``` creates internally a new thread which sends the export statements via ODBC to the server (B1). The server then starts to write the data in CSV format to the formerly created TCP socket. Thus, the client receives the data from the server via the socket (not via ODBC). 


### C level structure

The following diagram describes the high level architecture of the C level package.
![](./images/r-exasol-c_component.png)

R uses the plain C interface, which delegates all calls to class ```ConnectionContext```.
When initiating a new connection, ```ConnectionContext``` instantiates ```ConnectionController```, which opens the socket
and reads the meta-data.
After receiving ```asyncRODBCQueryStart```, the ```ConnectionController``` triggers the asynchronous ODBC call (```ODBCAsyncExecutor```) and creates a ```HttpChunkReader``` or ```HttpChunkWriter``` instance, which is returned to the ```ConnectionContext``` class.
ConnectionContext then create a specific rconnection class: ```RWriterConnection``` or ```RReaderConnection```, and passes the reader/writer obtained from ```ConnectionController``` to it. 
Finally, ```ConnectionContext``` returns the ```SEXP``` instance (which encapsulates the rconnection data structure) to the R client, which can start reading/writing. For more information please check [r-source](https://github.com/wch/r-source/blob/68251d4dd24b6bd970e5a6a92d5d07a3cf8a383d/src/main/connections.c).
The communication with ODBC requires access to the connection handle; we get access to this handle by reading the "handle_ptr" attribute in readData.R/writeData.R; please check [here](https://github.com/cran/RODBC/blob/59b0b93bd4947eb7c2c0223b0b45d56bc928d912/src/RODBC.c#L121).


#### OpenSSL Dependencies

##### Linux

On Linux we assume that OpenSSL has been installed (for example via ```sudo apt-get install libssl-dev``).
The build script (configure) checks if the header files and libraries are correctly installed.

The unit tests use CMake, CMake also finds the OpenSSL package on the system.

##### MacOS

t.b.d.

##### Windows

On Windows we leverage the [OpenSSL rwin-repository](https://github.com/rwinlib/openssl) for building the package.
It provides a static library for OpenSSL, this version is downloaded, unzipped and used for compilation.
