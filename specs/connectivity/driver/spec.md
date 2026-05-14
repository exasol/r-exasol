# Feature: Driver

The EXADriver object represents the Exasol database driver. No ODBC driver configuration is required on the system since connections use the WebSocket API protocol.

## Background

The default driver name is "EXASolution Driver". The driver is a lightweight object that does not require system-level ODBC configuration.

## Scenarios

### Scenario: Create driver without system ODBC driver

* *GIVEN* the system does not require an Exasol ODBC driver
* *WHEN* the user calls exasol() or new("EXADriver")
* *THEN* an EXADriver object SHALL be returned
* *AND* no ODBC driver configuration SHALL be required
