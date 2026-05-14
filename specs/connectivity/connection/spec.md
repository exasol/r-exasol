# Feature: Connection

All connections use the Exasol WebSocket API protocol. Connections use secure WebSocket (wss://) when TLS is enabled and plain WebSocket (ws://) otherwise. No ODBC driver is required on the system.

## Background

An Exasol database must be running and reachable. The WebSocket protocol v3 is used by default with optional v4 negotiation.

## Scenarios

### Scenario: Connect via hostname and credentials

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls dbConnect with host, port, username, and password
* *THEN* an EXAConnection object SHALL be returned
* *AND* the connection SHALL be established via WebSocket protocol
* *AND* the connection SHALL default to schema "SYS"
* *AND* encryption SHALL be enabled by default (wss://)

### Scenario: Connect with driver name as string

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls dbConnect with drv="exasol" or drv=exasol()
* *THEN* the system SHALL create a driver internally and return an EXAConnection object
* *AND* no ODBC driver SHALL be required on the system

### Scenario: Clone existing connection

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbConnect with an existing connection and overridden parameters
* *THEN* a new EXAConnection object SHALL be returned with a new WebSocket session
* *AND* the original connection SHALL remain unaffected

### Scenario: Connect with encryption disabled

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls dbConnect with encryption=FALSE
* *THEN* an EXAConnection object SHALL be returned using ws:// (unencrypted WebSocket)

### Scenario: Connect with default TLS certificate verification

* *GIVEN* an Exasol database with a valid TLS certificate is running and reachable
* *WHEN* the user calls dbConnect with encryption="Y" and omits sslcertificate (or passes "" or "SSL_VERIFY_SERVER")
* *THEN* the server certificate SHALL be verified against the system trust store
* *AND* the connection SHALL fail if the certificate is invalid

### Scenario: Connect with TLS certificate verification disabled

* *GIVEN* an Exasol database with a self-signed TLS certificate is running and reachable
* *WHEN* the user calls dbConnect with sslcertificate="SSL_VERIFY_NONE"
* *THEN* the server certificate SHALL NOT be verified
* *AND* the connection SHALL succeed regardless of certificate validity

### Scenario: Connect with custom TLS trust anchor

* *GIVEN* an Exasol database whose TLS certificate is signed by a non-system CA
* *WHEN* the user calls dbConnect with sslcertificate set to the path of a PEM trust file
* *THEN* the server certificate SHALL be verified against that PEM file
* *AND* the connection SHALL succeed if the certificate chains to the provided trust anchor

### Scenario: Apply query timeout to the session

* *GIVEN* an active connection has just been established
* *WHEN* the user calls dbConnect with querytimeout set to a non-negative integer (seconds)
* *THEN* the queryTimeout session attribute SHALL be set via the WebSocket setAttributes command
* *AND* a value of "0" SHALL disable the timeout (queries run until finished)

### Scenario: Password with semicolons

* *GIVEN* a user password contains semicolons
* *WHEN* the user provides the password via dbConnect
* *THEN* the password SHALL be encoded with curly braces in the connection string

### Scenario: Disconnect from database

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbDisconnect
* *THEN* a disconnect command SHALL be sent via WebSocket
* *AND* the WebSocket connection SHALL be closed
* *AND* the function SHALL return TRUE

### Scenario: Check connection validity

* *GIVEN* an EXAObject exists
* *WHEN* the user calls dbIsValid(dbObj)
* *THEN* the function SHALL return a logical value indicating validity

### Scenario: Get and set current schema

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbCurrentSchema(conn, setSchema = "NEW_SCHEMA")
* *THEN* the current schema SHALL be changed to "NEW_SCHEMA"
* *AND* the updated connection object SHALL be returned
