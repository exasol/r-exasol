# Feature: Driver

Enables R users to create Exasol ODBC driver instances for establishing database connections.

## Background

The default driver name is "EXASolution Driver". The driver must be configured on the system via ODBC.

## Scenarios

### Scenario: Create driver with system ODBC driver

* *GIVEN* the Exasol ODBC driver is configured on the system
* *WHEN* the user calls `exasol_driver()` or `exa()` without specifying a driver path
* *THEN* an EXADriver object SHALL be returned using the system "EXASolution Driver"

### Scenario: Create driver with explicit driver path

* *GIVEN* a valid Exasol ODBC driver file exists at a known path
* *WHEN* the user calls `exasol_driver(driver = "/path/to/driver.so")`
* *THEN* an EXADriver object SHALL be returned referencing the specified driver file
