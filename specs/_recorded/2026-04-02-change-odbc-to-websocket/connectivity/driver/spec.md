# Feature: Driver

The EXADriver object represents the Exasol database driver. No ODBC driver configuration is required on the system since connections use the WebSocket API protocol.

## Background

The default driver name is "EXASolution Driver". The driver is a lightweight object that does not require system-level ODBC configuration.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Create driver without system ODBC driver

* *GIVEN* the system does not require an Exasol ODBC driver
* *WHEN* the user calls exasol() or new("EXADriver")
* *THEN* an EXADriver object SHALL be returned
* *AND* no ODBC driver configuration SHALL be required
<!-- /DELTA:CHANGED -->

<!-- DELTA:REMOVED -->
### Scenario: Create driver with system ODBC driver

* *GIVEN* the Exasol ODBC driver is configured on the system
* *WHEN* the user calls exasol() without specifying a driver path
* *THEN* this scenario SHALL be removed because ODBC driver is no longer required
<!-- /DELTA:REMOVED -->

<!-- DELTA:REMOVED -->
### Scenario: Create driver with explicit driver path

* *GIVEN* a valid Exasol ODBC driver file exists at a known path
* *WHEN* the user calls exasol() with a driver path
* *THEN* this scenario SHALL be removed because ODBC driver paths are no longer used
<!-- /DELTA:REMOVED -->
