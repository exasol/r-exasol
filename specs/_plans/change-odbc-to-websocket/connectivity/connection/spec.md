# Feature: Connection

All connections use the Exasol WebSocket API protocol. Connections use secure WebSocket (wss://) when TLS is enabled and plain WebSocket (ws://) otherwise. No ODBC driver is required on the system.

## Background

An Exasol database must be running and reachable. The WebSocket protocol v3 is used by default with optional v4 negotiation.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Connect via hostname and credentials

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls dbConnect with host, port, username, and password
* *THEN* an EXAConnection object SHALL be returned
* *AND* the connection SHALL be established via WebSocket protocol
* *AND* the connection SHALL default to schema "SYS"
* *AND* encryption SHALL be enabled by default (wss://)
<!-- /DELTA:CHANGED -->

<!-- DELTA:REMOVED -->
### Scenario: Connect via DSN

* *GIVEN* an ODBC Data Source Name is configured on the system
* *WHEN* the user calls dbConnect with a DSN
* *THEN* this scenario SHALL be removed because ODBC/DSN support is eliminated
<!-- /DELTA:REMOVED -->

<!-- DELTA:REMOVED -->
### Scenario: Connect via custom connection string

* *GIVEN* an ODBC connection string exists
* *WHEN* the user calls dbConnect with a connection string
* *THEN* this scenario SHALL be removed because ODBC connection strings are eliminated
<!-- /DELTA:REMOVED -->

<!-- DELTA:CHANGED -->
### Scenario: Connect with driver name as string

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls dbConnect with drv="exasol" or drv=exasol()
* *THEN* the system SHALL create a driver internally and return an EXAConnection object
* *AND* no ODBC driver SHALL be required on the system
<!-- /DELTA:CHANGED -->

<!-- DELTA:CHANGED -->
### Scenario: Clone existing connection

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbConnect with an existing connection and overridden parameters
* *THEN* a new EXAConnection object SHALL be returned with a new WebSocket session
* *AND* the original connection SHALL remain unaffected
<!-- /DELTA:CHANGED -->

<!-- DELTA:CHANGED -->
### Scenario: Connect with encryption disabled

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls dbConnect with encryption=FALSE
* *THEN* an EXAConnection object SHALL be returned using ws:// (unencrypted WebSocket)
<!-- /DELTA:CHANGED -->

<!-- DELTA:CHANGED -->
### Scenario: Disconnect from database

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbDisconnect
* *THEN* a disconnect command SHALL be sent via WebSocket
* *AND* the WebSocket connection SHALL be closed
* *AND* the function SHALL return TRUE
<!-- /DELTA:CHANGED -->
