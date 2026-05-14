# Feature: Bulk Read

Bulk read uses EXPORT CSV AT protocol commands with an asynchronous C++ layer that opens HTTP/HTTPS connections directly to Exasol cluster nodes. The EXPORT query is triggered via WebSocket execute command. The protocol (http/https) is determined by the connection's encryption setting.

## Background

All bulk read operations require an authenticated WebSocket session and an active C++ HTTP/HTTPS channel.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Read data via high-speed channel

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls exa.readData
* *THEN* a data.frame SHALL be returned containing all matching rows
* *AND* the EXPORT INTO CSV AT query SHALL be sent via WebSocket execute command
* *AND* the data SHALL be transferred via the C++ HTTP/HTTPS channel
<!-- /DELTA:CHANGED -->
