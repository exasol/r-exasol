# Feature: Bulk Read

Bulk read uses EXPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The EXPORT query is triggered via WebSocket execute command. The protocol (http/https) is determined by the connection's encryption setting.

## Background

All bulk read operations require an authenticated WebSocket session and an active C++ HTTP/HTTPS channel.

## Scenarios

### Scenario: Read data via high-speed channel

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls exa.readData
* *THEN* a data.frame SHALL be returned containing all matching rows
* *AND* the EXPORT INTO CSV AT query SHALL be sent via WebSocket execute command
* *AND* the data SHALL be transferred via the C++ HTTP/HTTPS channel

### Scenario: Read data with custom encoding

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls exa.readData(conn, query, encoding = "latin1")
* *THEN* the result SHALL be decoded using the specified encoding

### Scenario: Read data with custom reader function

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls exa.readData(conn, query, reader = custom_reader_fn)
* *THEN* the custom reader function SHALL be called with the CSV data
* *AND* the return value of the reader function SHALL be returned

### Scenario: Read data over encrypted channel

* *GIVEN* an active EXAConnection exists with encryption enabled
* *WHEN* the user calls exa.readData(conn, query)
* *THEN* the transfer SHALL use HTTPS protocol

### Scenario: Read data over unencrypted channel

* *GIVEN* an active EXAConnection exists with encryption disabled
* *WHEN* the user calls exa.readData(conn, query)
* *THEN* the transfer SHALL use HTTP protocol

### Scenario: Read single-column result

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls exa.readData(conn, "SELECT single_col FROM table")
* *THEN* a data.frame with one column SHALL be returned
