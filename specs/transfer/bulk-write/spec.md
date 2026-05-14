# Feature: Bulk Write

Bulk write uses IMPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The IMPORT query is triggered via WebSocket execute command. The protocol (http/https) is determined by the connection's encryption setting.

## Background

All bulk write operations require an authenticated WebSocket session and an active C++ HTTP/HTTPS channel.

## Scenarios

### Scenario: Write data via high-speed channel

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists in the database
* *WHEN* the user calls exa.writeData
* *THEN* all rows from the data.frame SHALL be written to the table
* *AND* the IMPORT FROM CSV AT query SHALL be sent via WebSocket execute command
* *AND* the data SHALL be transferred via the C++ HTTP/HTTPS channel

### Scenario: Write data with column specification

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists with columns (col1, col2, col3)
* *WHEN* the user calls exa.writeData(conn, df, "table", tableColumns = c("col1", "col3"))
* *THEN* only the specified columns SHALL be populated

### Scenario: Write NULL or empty data frame

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls exa.writeData(conn, NULL, "table") or with a zero-row data.frame
* *THEN* the write operation SHALL be skipped
* *AND* the function SHALL return TRUE

### Scenario: Write data with custom writer function

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls exa.writeData(conn, df, "table", writer = custom_writer_fn)
* *THEN* the custom writer function SHALL be called to serialize the data

### Scenario: Write data without column names

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists
* *WHEN* the user calls exa.writeData(conn, df, "table") without specifying tableColumns
* *THEN* data SHALL be written matching columns by position
