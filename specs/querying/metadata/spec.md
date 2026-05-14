# Feature: Metadata

Metadata operations require an active EXAConnection. Table and field listing queries use high-speed data transfer via exa.readData for performance.

## Background

All metadata queries operate over an authenticated WebSocket session.

## Scenarios

### Scenario: List all tables

* *GIVEN* an active EXAConnection exists
* *AND* tables exist in the database
* *WHEN* the user calls dbListTables(conn)
* *THEN* a character vector SHALL be returned containing fully qualified table names (schema.table)

### Scenario: List tables filtered by schema

* *GIVEN* an active EXAConnection exists
* *AND* tables exist in multiple schemas
* *WHEN* the user calls dbListTables(conn, schema = "MY_SCHEMA")
* *THEN* only tables from "MY_SCHEMA" SHALL be returned

### Scenario: List fields of a table

* *GIVEN* an active EXAConnection exists
* *AND* a table exists in the database
* *WHEN* the user calls dbListFields(conn, "table_name")
* *THEN* a character vector SHALL be returned containing all column names

### Scenario: List fields with schema-qualified table name

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbListFields(conn, "schema.table_name")
* *THEN* the system SHALL parse the schema from the qualified name
* *AND* a character vector of column names SHALL be returned

### Scenario: Check table existence

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbExistsTable(conn, "table_name")
* *THEN* the function SHALL return TRUE if the table exists
* *AND* the function SHALL return FALSE if the table does not exist

### Scenario: Get driver info

* *GIVEN* an EXADriver object exists
* *WHEN* the user calls dbGetInfo on the driver
* *THEN* a named list SHALL be returned containing driver.version, max.connections, DBI.version, and client.version
* *AND* the RODBC.version field SHALL NOT be present

### Scenario: Get connection info

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbGetInfo(conn)
* *THEN* a named list SHALL be returned containing db.version, dbname, username, host, and port

### Scenario: Get result info

* *GIVEN* an EXAResult exists
* *WHEN* the user calls dbGetInfo(res)
* *THEN* a named list SHALL be returned containing statement, row.count, rows.affected, has.completed, and is.select
