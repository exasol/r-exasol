# Feature: Bulk Data Transfer

Enables high-speed parallel data transfer between R and Exasol using native C++ HTTP/HTTPS channels, bypassing standard ODBC row-by-row transfer for significantly higher throughput on multinode clusters.

## Background

Bulk transfer uses EXPORT/IMPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The protocol (http/https) is determined by the connection's encryption setting. All transfers use IGNORE CERTIFICATE for SSL.

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

### Scenario: Write data via high-speed channel

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists in the database
* *WHEN* the user calls `exa.writeData(conn, df, "schema.table")`
* *THEN* all rows from the data.frame SHALL be written to the table
* *AND* the data SHALL be transferred via IMPORT FROM CSV AT using the C++ channel

### Scenario: Write data with column specification

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists with columns (col1, col2, col3)
* *WHEN* the user calls `exa.writeData(conn, df, "table", tableColumns = c("col1", "col3"))`
* *THEN* only the specified columns SHALL be populated

### Scenario: Write NULL or empty data frame

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `exa.writeData(conn, NULL, "table")` or with a zero-row data.frame
* *THEN* the write operation SHALL be skipped
* *AND* the function SHALL return TRUE

### Scenario: Write data with custom writer function

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `exa.writeData(conn, df, "table", writer = custom_writer_fn)`
* *THEN* the custom writer function SHALL be called to serialize the data

### Scenario: Read single-column result

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `exa.readData(conn, "SELECT single_col FROM table")`
* *THEN* a data.frame with one column SHALL be returned

### Scenario: Write data without column names

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists
* *WHEN* the user calls `exa.writeData(conn, df, "table")` without specifying tableColumns
* *THEN* data SHALL be written matching columns by position
