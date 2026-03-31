# Feature: Metadata

Metadata operations require an active EXAConnection. Table and field listing queries use high-speed data transfer via exa.readData for performance.

## Background

All metadata queries operate over an authenticated WebSocket session.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Get driver info

* *GIVEN* an EXADriver object exists
* *WHEN* the user calls dbGetInfo on the driver
* *THEN* a named list SHALL be returned containing driver.version, max.connections, DBI.version, and client.version
* *AND* the RODBC.version field SHALL NOT be present
<!-- /DELTA:CHANGED -->
