# Feature: Query Execution

Query execution operates over an active EXAConnection. SELECT queries use high-speed data transfer by default via exa.readData. Non-SELECT statements are executed via the WebSocket execute command. Result sets from dbSendQuery are stored in temporary tables for incremental fetching.

## Background

All query execution requires an authenticated WebSocket session.

## Scenarios

### Scenario: Execute SELECT and retrieve full result

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls dbGetQuery(conn, "SELECT * FROM schema.table")
* *THEN* a data.frame SHALL be returned containing all matching rows
* *AND* the high-speed data transfer channel SHALL be used

### Scenario: Execute non-SELECT statement

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbSendQuery or dbExecute with a non-SELECT statement
* *THEN* the statement SHALL be executed via WebSocket execute command
* *AND* the result SHALL indicate success or failure

### Scenario: Handle SQL comment before SELECT

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbGetQuery(conn, "/* comment */ SELECT ...")
* *THEN* the system SHALL correctly identify the statement as a SELECT
* *AND* a data.frame SHALL be returned

### Scenario: Handle comment before SELECT with CTE

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls dbGetQuery(conn, "/* comment */ WITH cte AS (...) SELECT ...")
* *THEN* the system SHALL correctly identify the statement as a SELECT
* *AND* a data.frame SHALL be returned

### Scenario: Send query and fetch incrementally

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls dbSendQuery(conn, "SELECT * FROM schema.table")
* *THEN* an EXAResult object SHALL be returned
* *AND* no data SHALL be transferred yet

### Scenario: Fetch specific number of rows

* *GIVEN* an EXAResult from a SELECT query exists
* *WHEN* the user calls dbFetch(res, n = 10)
* *THEN* a data.frame with at most 10 rows SHALL be returned
* *AND* dbHasCompleted(res) SHALL return FALSE if rows remain

### Scenario: Fetch all remaining rows

* *GIVEN* an EXAResult from a SELECT query exists
* *WHEN* the user calls dbFetch(res, n = -1)
* *THEN* a data.frame with all remaining rows SHALL be returned
* *AND* dbHasCompleted(res) SHALL return TRUE

### Scenario: Check result completion status

* *GIVEN* an EXAResult exists
* *WHEN* the user calls dbHasCompleted(res)
* *THEN* the function SHALL return TRUE if all rows have been fetched
* *AND* the function SHALL return FALSE otherwise

### Scenario: Get rows affected count

* *GIVEN* an EXAResult exists
* *WHEN* the user calls dbGetRowsAffected(res)
* *THEN* the total number of rows in the result set SHALL be returned

### Scenario: Clear result and free resources

* *GIVEN* an EXAResult exists with a temporary table
* *WHEN* the user calls dbClearResult(res)
* *THEN* the temporary table SHALL be dropped
* *AND* the function SHALL return TRUE
