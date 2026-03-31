# Feature: Bulk Write

Bulk write uses IMPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The IMPORT query is triggered via WebSocket execute command. The protocol (http/https) is determined by the connection's encryption setting.

## Background

All bulk write operations require an authenticated WebSocket session and an active C++ HTTP/HTTPS channel.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Write data via high-speed channel

* *GIVEN* an active EXAConnection exists
* *AND* a target table exists in the database
* *WHEN* the user calls exa.writeData
* *THEN* all rows from the data.frame SHALL be written to the table
* *AND* the IMPORT FROM CSV AT query SHALL be sent via WebSocket execute command
* *AND* the data SHALL be transferred via the C++ HTTP/HTTPS channel
<!-- /DELTA:CHANGED -->
