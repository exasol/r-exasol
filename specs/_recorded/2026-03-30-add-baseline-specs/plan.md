# Plan: add-baseline-specs

## Summary

Spec out the existing behavior of all four core capabilities of r-exasol (connectivity, querying, bulk transfer, transactions) to establish a baseline for regression testing and future development.

## Features

| Feature | Status | Spec |
|---------|--------|------|
| Driver and Connection | NEW | `connectivity/driver-and-connection/spec.md` |
| Query Execution | NEW | `querying/query-execution/spec.md` |
| Table Operations | NEW | `querying/table-operations/spec.md` |
| Metadata | NEW | `querying/metadata/spec.md` |
| Bulk Data Transfer | NEW | `transfer/bulk-data-transfer/spec.md` |
| Transaction Management | NEW | `transactions/transaction-management/spec.md` |

## Implementation Tasks

1. Record all spec deltas to permanent specs via `/speq:record`
2. Map remaining uncovered scenarios to new integration tests in `tests/testthat/`
3. Ensure all existing tests pass with `devtools::test()` inside Docker environment

## Verification

### Scenario Coverage

| Scenario | Test Type | Test Location | Test Name |
|----------|-----------|---------------|-----------|
| **connectivity/driver-and-connection** | | | |
| Create driver with system ODBC driver | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest driver tests) |
| Create driver with explicit driver path | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest driver tests) |
| Connect via hostname and credentials | Integration | `tests/testthat/test-Connection.R` | `encryption_true` |
| Connect via DSN | Integration | — | Not yet covered |
| Connect via custom connection string | Integration | — | Not yet covered |
| Connect with driver name as string | Integration | `tests/testthat/test-Connection.R` | `encryption_true` |
| Clone existing connection | Integration | `tests/testthat/test-Connection.R` | `cloned_connection` |
| Connect with encryption disabled | Integration | `tests/testthat/test-Connection.R` | `connection_no_attributes` |
| Password with semicolons | Unit | `tests/testthat/test-Encode-Password.R` | `encode_password` |
| Disconnect from database | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest connection tests) |
| Check connection validity | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest connection tests) |
| Get and set current schema | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbCurrentSchema` |
| **querying/query-execution** | | | |
| Execute SELECT and retrieve full result | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| Execute non-SELECT statement | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| Handle SQL comment before SELECT | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbGetQuery_comment_before_select_stmt` |
| Handle comment before SELECT with CTE | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbGetQuery_comment_before_select_stmt_with_CTE` |
| Send query and fetch incrementally | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| Fetch specific number of rows | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| Fetch all remaining rows | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| Check result completion status | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbHasCompleted` |
| Get rows affected count | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| Clear result and free resources | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| **querying/table-operations** | | | |
| Read entire table into data frame | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbReadTable` |
| Read table with ordering and limit | Integration | — | Not yet covered |
| Write data frame to new table | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbWriteTable` |
| Write data frame with explicit column types | Integration | — | Not yet covered |
| Write data frame with overwrite | Integration | — | Not yet covered |
| Append data frame to existing table | Integration | — | Not yet covered |
| Write empty data frame | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbWriteTable_empty_df_skip` |
| Write data frame with specified column mapping | Integration | — | Not yet covered |
| Remove existing table | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest cleanup) |
| Remove table with cascade | Integration | — | Not yet covered |
| Map R data types to Exasol SQL types | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbDataType_empty_char`, `test_dbDataType_non_empty_char` |
| **querying/metadata** | | | |
| List all tables | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest) |
| List tables filtered by schema | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbListTables_with_schema_filter` |
| List fields of a table | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest) |
| List fields with schema-qualified table name | Integration | — | Not yet covered |
| Check table existence | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest) |
| Get driver info | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest driver tests) |
| Get connection info | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest connection tests) |
| Get result info | Integration | `tests/testthat/run-dbi-tests.R` | (covered by DBItest result tests) |
| **transfer/bulk-data-transfer** | | | |
| Read data via high-speed channel | Integration | `tests/testthat/run-dbi-tests.R` | `test_exa.readData_multiple_cols` |
| Read data with custom encoding | Integration | — | Not yet covered |
| Read data with custom reader function | Integration | — | Not yet covered |
| Read data over encrypted channel | Integration | `tests/testthat/test-Connection.R` | `encryption_true` (implicitly) |
| Read data over unencrypted channel | Integration | `tests/testthat/test-Connection.R` | `connection_no_attributes` (implicitly) |
| Write data via high-speed channel | Integration | `tests/testthat/run-dbi-tests.R` | `test_exa.writeData_multiple_cols` |
| Write data with column specification | Integration | `tests/testthat/run-dbi-tests.R` | `test_exa.readData_single_col` |
| Write NULL or empty data frame | Integration | `tests/testthat/run-dbi-tests.R` | `test_dbWriteTable_empty_df_skip` |
| Write data with custom writer function | Integration | — | Not yet covered |
| Read single-column result | Integration | `tests/testthat/run-dbi-tests.R` | `test_exa.readData_single_col` |
| Write data without column names | Integration | `tests/testthat/run-dbi-tests.R` | `test_exa.writeData_without_col_names` |
| **transactions/transaction-management** | | | |
| Begin a transaction | Integration | — | Not yet covered |
| Commit a transaction | Integration | — | Not yet covered |
| Rollback a transaction | Integration | — | Not yet covered |
| End transaction with commit | Integration | — | Not yet covered |
| End transaction with rollback | Integration | — | Not yet covered |

### Manual Testing

| Feature | Command | Expected Output |
|---------|---------|-----------------|
| Driver and Connection | `library(exasol); con <- dbConnect("exa", exahost="localhost:8563", uid="sys", pwd="exasol")` | EXAConnection object created, no errors |
| Query Execution | `dbGetQuery(con, "SELECT 1 AS test_col")` | data.frame with 1 row, column test_col = 1 |
| Table Operations | `dbWriteTable(con, "TEST_TBL", data.frame(a=1:3, b=c("x","y","z")), schema="SYS"); dbReadTable(con, "TEST_TBL", schema="SYS")` | data.frame with 3 rows matching input |
| Metadata | `dbListTables(con); dbListFields(con, "TEST_TBL", schema="SYS")` | Character vectors of table names and column names |
| Bulk Data Transfer | `exa.readData(con, "SELECT * FROM SYS.TEST_TBL")` | data.frame identical to dbReadTable result |
| Transaction Management | `dbBegin(con); dbGetQuery(con, "INSERT INTO ..."); dbRollback(con)` | Insert is rolled back, table unchanged |

### Checklist

| Step | Command | Expected |
|------|---------|----------|
| Build | `devtools::build()` | Exit 0 |
| Test | `devtools::test()` | 0 failures |
| R CMD check | `R CMD build . && R CMD check exasol_*.tar.gz` | 0 errors, 0 warnings |
