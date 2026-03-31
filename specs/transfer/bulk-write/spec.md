# Feature: Bulk Write

Enables high-speed parallel writing of data from R into Exasol using native C++ HTTP/HTTPS channels via IMPORT FROM CSV AT, bypassing standard ODBC row-by-row transfer.

## Background

Bulk write uses IMPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The protocol (http/https) is determined by the connection's encryption setting. All transfers use IGNORE CERTIFICATE for SSL.

## Scenarios

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

### Scenario: Write data without column names

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists
* *WHEN* the user calls `exa.writeData(conn, df, "table")` without specifying tableColumns
* *THEN* data SHALL be written matching columns by position
