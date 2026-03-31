# Feature: Table Operations

Enables R users to read, write, and remove database tables through DBI-compliant methods, with automatic type mapping between R data types and Exasol SQL types.

## Background

Table operations require an active EXAConnection. Read operations use high-speed data transfer via `exa.readData`. Write operations use `exa.writeData` for bulk transfer. The system automatically maps R data types to Exasol SQL types when creating tables.

## Scenarios

### Scenario: Read entire table into data frame

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls `dbReadTable(conn, "table_name", schema = "schema")`
* *THEN* a data.frame SHALL be returned containing all rows from the table

### Scenario: Read table with ordering and limit

* *GIVEN* an active EXAConnection exists
* *AND* a table with data exists in the database
* *WHEN* the user calls `dbReadTable(conn, "table_name", order_col = "col1 desc", limit = 10)`
* *THEN* a data.frame SHALL be returned with at most 10 rows
* *AND* the rows SHALL be ordered by col1 descending

### Scenario: Write data frame to new table

* *GIVEN* an active EXAConnection exists
* *AND* the target table does not exist
* *WHEN* the user calls `dbWriteTable(conn, "table_name", df, schema = "schema")`
* *THEN* the table SHALL be created with types inferred from the data.frame
* *AND* all rows from the data.frame SHALL be written to the table

### Scenario: Write data frame with explicit column types

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `dbWriteTable(conn, "table_name", df, field_types = c("VARCHAR(20)", "INT"))`
* *THEN* the table SHALL be created with the specified column types

### Scenario: Write data frame with overwrite

* *GIVEN* an active EXAConnection exists
* *AND* the target table already exists with data
* *WHEN* the user calls `dbWriteTable(conn, "table_name", df, overwrite = TRUE)`
* *THEN* the existing data SHALL be truncated
* *AND* the new data SHALL be written

### Scenario: Append data frame to existing table

* *GIVEN* an active EXAConnection exists
* *AND* the target table already exists with data
* *WHEN* the user calls `dbWriteTable(conn, "table_name", df, overwrite = FALSE)`
* *THEN* the new data SHALL be appended to the existing data

### Scenario: Write empty data frame

* *GIVEN* an active EXAConnection exists
* *WHEN* the user calls `dbWriteTable(conn, "table_name", empty_df)`
* *AND* the data.frame has zero rows
* *THEN* the write operation SHOULD be skipped
* *AND* the function SHALL return TRUE

### Scenario: Write data frame with specified column mapping

* *GIVEN* an active EXAConnection exists
* *AND* the target table exists with columns (col1, col2, col3, col4)
* *WHEN* the user calls `dbWriteTable(conn, "table_name", df, writeCols = c("col1", "col4", "col3"))`
* *THEN* the data SHALL be written to the specified columns only

### Scenario: Remove existing table

* *GIVEN* an active EXAConnection exists
* *AND* a table exists in the database
* *WHEN* the user calls `dbRemoveTable(conn, "table_name")`
* *THEN* the table SHALL be dropped from the database
* *AND* the function SHALL return TRUE

### Scenario: Remove table with cascade

* *GIVEN* an active EXAConnection exists
* *AND* a table exists with dependent constraints
* *WHEN* the user calls `dbRemoveTable(conn, "table_name", cascade = TRUE)`
* *THEN* the table SHALL be dropped with CASCADE CONSTRAINTS

### Scenario: Map R data types to Exasol SQL types

* *GIVEN* an EXADriver or EXAConnection exists
* *WHEN* the user calls `dbDataType(dbObj, obj)` with an R object
* *THEN* integer values SHALL map to "INT"
* *AND* numeric values SHALL map to "DECIMAL(36,15)"
* *AND* logical values SHALL map to "BOOLEAN"
* *AND* Date values SHALL map to "DATE"
* *AND* POSIXct values SHALL map to "TIMESTAMP"
* *AND* character values SHALL map to "VARCHAR(n)" where n is the max string length
* *AND* empty character values SHALL map to "CLOB"
