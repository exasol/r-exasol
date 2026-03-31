# Feature: Bulk Read

Enables high-speed parallel reading of data from Exasol into R using native C++ HTTP/HTTPS channels via EXPORT INTO CSV AT, bypassing standard ODBC row-by-row transfer.

## Background

Bulk read uses EXPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The protocol (http/https) is determined by the connection's encryption setting. All transfers use IGNORE CERTIFICATE for SSL.

## Scenarios

### Scenario: Read data via high-speed channel

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls `exa.readData(conn, "SELECT * FROM schema.table")`
* *THEN* a data.frame SHALL be returned containing all matching rows
* *AND* the data SHALL be transferred via EXPORT INTO CSV AT using the C++ channel

### Scenario: Read data with custom encoding

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `exa.readData(conn, query, encoding = "latin1")`
* *THEN* the result SHALL be decoded using the specified encoding

### Scenario: Read data with custom reader function

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `exa.readData(conn, query, reader = custom_reader_fn)`
* *THEN* the custom reader function SHALL be called with the CSV data
* *AND* the return value of the reader function SHALL be returned

### Scenario: Read data over encrypted channel

* *GIVEN* an active EXAConnection exists with encryption enabled
* *WHEN* the user calls `exa.readData(conn, query)`
* *THEN* the transfer SHALL use HTTPS protocol

### Scenario: Read data over unencrypted channel

* *GIVEN* an active EXAConnection exists with encryption disabled
* *WHEN* the user calls `exa.readData(conn, query)`
* *THEN* the transfer SHALL use HTTP protocol

### Scenario: Read single-column result

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `exa.readData(conn, "SELECT single_col FROM table")`
* *THEN* a data.frame with one column SHALL be returned
