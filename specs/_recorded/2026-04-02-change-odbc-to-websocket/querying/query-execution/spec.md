# Feature: Query Execution

Query execution operates over an active EXAConnection. SELECT queries use high-speed data transfer by default via exa.readData. Non-SELECT statements are executed via the WebSocket execute command. Result sets from dbSendQuery are stored in temporary tables for incremental fetching.

## Background

All query execution requires an authenticated WebSocket session.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Execute non-SELECT statement

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbSendQuery or dbExecute with a non-SELECT statement
* *THEN* the statement SHALL be executed via WebSocket execute command
* *AND* the result SHALL indicate success or failure
<!-- /DELTA:CHANGED -->
