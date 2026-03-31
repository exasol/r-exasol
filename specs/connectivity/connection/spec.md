# Feature: Connection

Enables R users to establish DBI-compliant connections to Exasol databases via ODBC, supporting multiple connection methods, encryption, and lifecycle management.

## Background

All connections require a configured Exasol ODBC driver on the system. Connections use RODBC internally and support both encrypted (default) and unencrypted channels.

## Scenarios

### Scenario: Connect via hostname and credentials

* *GIVEN* an EXADriver object exists
* *AND* an Exasol database is running and reachable
* *WHEN* the user calls `dbConnect(drv, exahost = "host:port", uid = "user", pwd = "password")`
* *THEN* an EXAConnection object SHALL be returned
* *AND* the connection SHALL default to schema "SYS"
* *AND* encryption SHALL be enabled by default

### Scenario: Connect via DSN

* *GIVEN* an ODBC Data Source Name is configured on the system
* *AND* an Exasol database is running and reachable
* *WHEN* the user calls `dbConnect("exa", dsn = "MyDSN")`
* *THEN* an EXAConnection object SHALL be returned using the DSN configuration

### Scenario: Connect via custom connection string

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls `dbConnect("exa", connection_string = "...")`
* *THEN* an EXAConnection object SHALL be returned using the provided connection string

### Scenario: Connect with driver name as string

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls `dbConnect("exa", ...)` or `dbConnect("exasol", ...)`
* *THEN* the system SHALL create a driver internally and return an EXAConnection object

### Scenario: Clone existing connection

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `dbConnect(existingConnection, schema = "NEW_SCHEMA")`
* *THEN* a new EXAConnection object SHALL be returned with overridden parameters
* *AND* the original connection SHALL remain unaffected

### Scenario: Connect with encryption disabled

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the user calls `dbConnect(drv, ..., encryption = "N")`
* *THEN* an EXAConnection object SHALL be returned with `encrypted = FALSE`

### Scenario: Password with semicolons

* *GIVEN* a user password contains semicolons
* *WHEN* the user provides the password via `dbConnect`
* *THEN* the password SHALL be encoded with curly braces in the connection string

### Scenario: Disconnect from database

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `dbDisconnect(conn)`
* *THEN* the connection SHALL be closed
* *AND* the function SHALL return TRUE

### Scenario: Check connection validity

* *GIVEN* an EXAObject exists
* *WHEN* the user calls `dbIsValid(dbObj)`
* *THEN* the function SHALL return a logical value indicating validity

### Scenario: Get and set current schema

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `dbCurrentSchema(conn, setSchema = "NEW_SCHEMA")`
* *THEN* the current schema SHALL be changed to "NEW_SCHEMA"
* *AND* the updated connection object SHALL be returned
