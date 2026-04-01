# Plan: change-odbc-to-websocket

## Summary

Replace the RODBC/ODBC dependency with Exasol's native WebSocket API (JSON-over-WebSocket protocol v3) for all command-channel operations (connect, query, transactions, metadata), eliminating the requirement for an ODBC driver on the system. The existing C++ HTTP/HTTPS bulk data transfer layer is retained for high-speed parallel read/write.

## Design

### Context

The current architecture requires an Exasol ODBC driver and the RODBC R package for all database communication. This creates a heavy system-level dependency that complicates installation across platforms. Exasol provides a documented WebSocket API that enables direct JSON-over-WebSocket communication, which other drivers (PyExasol, Go driver) already use successfully.

- **Goals** — Remove ODBC/RODBC dependency; connect directly via WebSocket protocol; maintain DBI compliance; keep high-speed bulk transfer performance; use Rcpp for the R-to-C++ bridge (replacing raw `.Call`/`SEXP` interfaces)
- **Non-Goals** — Replacing the C++ HTTP/HTTPS bulk transfer with WebSocket subconnections; supporting ODBC as a fallback; implementing WebSocket API v4 features (only negotiate v4 if server offers it)

### Decision

#### Architecture

```
R DBI Layer (S4 classes: EXADriver, EXAConnection, EXAResult)
         │
         ├── WebSocket command channel (R or C++)
         │     ├── JSON serialization/deserialization
         │     ├── RSA authentication (OpenSSL)
         │     └── ws:// or wss:// transport
         │
         └── C++ HTTP/HTTPS bulk transfer (unchanged)
               ├── EXPORT INTO CSV AT (read)
               └── IMPORT FROM CSV AT (write)
```

The WebSocket layer replaces all RODBC calls:
- `odbcDriverConnect` → WebSocket connect + login command
- `sqlQuery` → WebSocket execute command
- `odbcEndTran` / `odbcSetAutoCommit` → WebSocket execute (COMMIT/ROLLBACK) + setAttributes
- `odbcGetInfo` → Login response metadata (sessionId, releaseVersion, databaseName, etc.)
- `odbcClose` → WebSocket disconnect command
- `odbcGetErrMsg` → Exception parsing from JSON error responses

For bulk transfer, the C++ async query executor (`OdbcQueryExecutor`) currently uses ODBC handles (`SQLAllocHandle`, `SQLExecDirect`) to trigger EXPORT/IMPORT queries. This must be replaced with WebSocket execute commands to trigger the same EXPORT/IMPORT SQL, while the HTTP data channel remains unchanged.

#### Patterns

| Pattern | Where | Why |
|---------|-------|-----|
| JSON command/response | WebSocket layer | Exasol WebSocket API protocol requirement |
| RSA public key auth | Login flow | Protocol-mandated password encryption (PKCS#1 v1.5 + Base64) |
| Connection object composition | EXAConnection | Replace RODBC superclass with WebSocket handle slot |
| Async query via WebSocket | Bulk transfer trigger | Replace OdbcQueryExecutor's ODBC calls with WebSocket execute |

### Consequences

| Decision | Alternatives Considered | Rationale |
|----------|------------------------|-----------|
| WebSocket in C++ layer | Pure R (websocket pkg) | C++ gives control over async execution needed for bulk transfer trigger; reuses existing OpenSSL linkage for wss:// and RSA auth |
| Keep HTTP bulk transfer | WebSocket subconnections | HTTP/CSV streaming is faster than JSON-encoded WebSocket data for large datasets |
| Clean break from ODBC | Dual-mode (ODBC fallback) | Maintaining two connection backends doubles complexity; WebSocket API is stable since Exasol 7.1 |
| Protocol v3 default | v4 only, or v1/v2 | v3 covers Exasol 7.1+ (broad compatibility); v4 negotiation is additive |

## Features

| Feature | Status | Spec |
|---------|--------|------|
| WebSocket protocol | NEW | `connectivity/websocket-protocol/spec.md` |
| Connection | CHANGED | `connectivity/connection/spec.md` |
| Driver | CHANGED | `connectivity/driver/spec.md` |
| Query execution | CHANGED | `querying/query-execution/spec.md` |
| Metadata | CHANGED | `querying/metadata/spec.md` |
| Transaction management | CHANGED | `transactions/transaction-management/spec.md` |
| Bulk read | CHANGED | `transfer/bulk-read/spec.md` |
| Bulk write | CHANGED | `transfer/bulk-write/spec.md` |

## Dependencies

| Dependency | Purpose | Replaces |
|------------|---------|----------|
| WebSocket C++ library (e.g. libwebsockets, Boost.Beast, or ixwebsocket) | WebSocket transport | RODBC / ODBC driver |
| OpenSSL (already linked) | wss:// TLS + RSA authentication | Already used for HTTPS bulk transfer |
| JSON library (e.g. nlohmann/json or RapidJSON) | Command serialization/deserialization | N/A (new) |

## Migration

| Current | New |
|---------|-----|
| `EXAConnection` extends `RODBC` superclass | `EXAConnection` holds WebSocket handle as slot |
| `odbcDriverConnect(con_str)` | WebSocket connect + login JSON command |
| `sqlQuery(con, sql)` | WebSocket execute JSON command |
| `odbcEndTran(con, commit)` | WebSocket execute COMMIT/ROLLBACK |
| `odbcSetAutoCommit(con, flag)` | WebSocket setAttributes `{autocommit: flag}` |
| `odbcGetInfo(con)` | Cached login response metadata |
| `odbcGetErrMsg(con)` | JSON exception parsing from response |
| `odbcClose(con)` | WebSocket disconnect command + close |
| `OdbcQueryExecutor` (C++ ODBC handles) | WebSocket execute for EXPORT/IMPORT trigger |
| ODBC connection string parameters | WebSocket URL + login attributes |
| `DESCRIPTION` Imports: RODBC | Remove RODBC dependency |

## Dead Code Removal

| Type | Location | Reason |
|------|----------|--------|
| R import | `R/exasol.R` (`@import RODBC`) | RODBC no longer used |
| Namespace | `NAMESPACE` (`import(RODBC)`) | RODBC no longer used |
| C++ header | `src/r_exasol/external/r_odbc.h` | ODBC handle struct no longer needed |
| C++ header | `src/r_exasol/external/r_odbc_fwd.h` | ODBC forward declarations no longer needed |
| C++ source | `src/r_exasol/odbc/odbc_query_executor.h/.cpp` | Replaced by WebSocket query executor |
| C++ source | `src/r_exasol/odbc/odbc_session_info_impl.h/.cpp` | Replaced by WebSocket session info |
| C bridge | `src/exasol.c` (RODBC handle extraction) | No RODBC handles to extract |
| DESCRIPTION | `Imports: RODBC (>= 1.3-12)` | Dependency removed |
| Connection string builder | `R/EXADBI-connection.R` (ODBC string logic) | Replaced by WebSocket URL + login |
| DSN connection path | `R/EXADBI-connection.R` (DSN scenario) | DSN support removed |

## Implementation Tasks

1. Select and integrate a C++ WebSocket library (libwebsockets, Boost.Beast, or ixwebsocket) into the build system (configure.ac, Makevars)
2. Select and integrate a C++ JSON library (nlohmann/json or RapidJSON) into the build system
3. Implement C++ WebSocket transport layer: connect (ws/wss), send/receive JSON frames, close
4. Implement C++ authentication flow: login command, RSA public key parsing, password encryption (PKCS#1 v1.5), Base64 encoding, credential submission
5. Implement C++ command layer: execute, fetch, disconnect, setAttributes, getAttributes, closeResultSet
6. Implement C++ error handling: parse JSON error responses with sqlCode and text
7. Create R-to-C++ bridge functions (`.Call`) for WebSocket operations: connect, login, execute, fetch, disconnect, set/get attributes
8. Refactor `EXAConnection` S4 class: remove RODBC superclass, add WebSocket handle slot, store session metadata from login response
9. Refactor `EXADriver` S4 class: remove ODBC driver path logic
10. Rewrite `.EXANewConnection` in `R/EXADBI-connection.R`: WebSocket connect + login instead of `odbcDriverConnect`
11. Rewrite query execution in `R/EXADBI-query.R`: WebSocket execute instead of `sqlQuery`
12. Rewrite transaction management in `R/EXADBI-transaction.R`: WebSocket execute + setAttributes instead of `odbcEndTran`/`odbcSetAutoCommit`
13. Rewrite table operations in `R/EXADBI-table.R`: WebSocket execute instead of `sqlQuery`
14. Replace `OdbcQueryExecutor` in C++ bulk transfer path: use WebSocket execute to trigger EXPORT/IMPORT queries instead of `SQLAllocHandle`/`SQLExecDirect`
15. Update `R/EXADBI-info.R`: remove RODBC.version from driver info, use login response for connection info
16. Remove RODBC from DESCRIPTION Imports and NAMESPACE
17. Remove dead ODBC code (see Dead Code Removal table)
18. Update configure.ac to link WebSocket and JSON libraries instead of requiring ODBC headers
19. Write integration tests for WebSocket protocol scenarios
20. Write integration tests for changed connection, query, transaction, metadata, and bulk transfer scenarios
21. Update documentation and examples to reflect WebSocket-based connection API

## Parallelization

| Parallel Group | Tasks |
|----------------|-------|
| Group A: C++ libraries | Task 1, Task 2 |
| Group B: C++ WebSocket core | Task 3, Task 4, Task 5, Task 6 |
| Group C: R bridge + refactor | Task 7, Task 8, Task 9 |
| Group D: R layer rewrite | Task 10, Task 11, Task 12, Task 13, Task 14, Task 15 |
| Group E: Cleanup | Task 16, Task 17, Task 18 |
| Group F: Testing | Task 19, Task 20, Task 21 |

Sequential dependencies:
- Group A → Group B (C++ WebSocket needs libraries)
- Group B → Group C (R bridge needs C++ layer)
- Group C → Group D (R rewrites need bridge functions)
- Group D → Group E (cleanup after rewrite)
- Group D → Group F (tests need working implementation)

## Verification

### Scenario Coverage

| Scenario | Test Type | Test Location | Test Name |
|----------|-----------|---------------|-----------|
| Establish WebSocket connection | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("WebSocket connection is established")` |
| Authenticate with credentials | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("authentication succeeds with valid credentials")` |
| Handle authentication failure | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("authentication fails with invalid credentials")` |
| Send command and receive response | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("execute command returns response")` |
| Handle error response | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("error response raises R error with sqlCode")` |
| Negotiate protocol version | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("protocol version is negotiated")` |
| Close WebSocket connection | Integration | `tests/testthat/test-websocket-protocol.R` | `test_that("disconnect closes WebSocket cleanly")` |
| Connect via hostname and credentials | Integration | `tests/testthat/test-connection.R` | `test_that("dbConnect creates connection via WebSocket")` |
| Connect with driver name as string | Integration | `tests/testthat/test-connection.R` | `test_that("dbConnect with string driver works without ODBC")` |
| Clone existing connection | Integration | `tests/testthat/test-connection.R` | `test_that("dbConnect clones connection with new WebSocket session")` |
| Connect with encryption disabled | Integration | `tests/testthat/test-connection.R` | `test_that("dbConnect with encryption=FALSE uses ws://")` |
| Disconnect from database | Integration | `tests/testthat/test-connection.R` | `test_that("dbDisconnect sends disconnect and closes WebSocket")` |
| Create driver without system ODBC | Integration | `tests/testthat/test-driver.R` | `test_that("exasol() creates driver without ODBC")` |
| Execute non-SELECT statement | Integration | `tests/testthat/test-query.R` | `test_that("non-SELECT executes via WebSocket")` |
| Get driver info | Integration | `tests/testthat/test-metadata.R` | `test_that("dbGetInfo on driver has no RODBC.version")` |
| Begin a transaction | Integration | `tests/testthat/test-transaction.R` | `test_that("dbBegin disables autocommit via WebSocket")` |
| Commit a transaction | Integration | `tests/testthat/test-transaction.R` | `test_that("dbCommit sends COMMIT via WebSocket")` |
| Rollback a transaction | Integration | `tests/testthat/test-transaction.R` | `test_that("dbRollback sends ROLLBACK via WebSocket")` |
| End transaction with commit | Integration | `tests/testthat/test-transaction.R` | `test_that("dbEnd with commit restores autocommit")` |
| End transaction with rollback | Integration | `tests/testthat/test-transaction.R` | `test_that("dbEnd with rollback restores autocommit")` |
| Read data via high-speed channel | Integration | `tests/testthat/test-bulk-read.R` | `test_that("exa.readData triggers EXPORT via WebSocket")` |
| Write data via high-speed channel | Integration | `tests/testthat/test-bulk-write.R` | `test_that("exa.writeData triggers IMPORT via WebSocket")` |

### Manual Testing

| Feature | Command | Expected Output |
|---------|---------|-----------------|
| WebSocket protocol | `con <- dbConnect(exasol(), host="localhost", port=8563, uid="sys", pwd="exasol")` | Connection established, no ODBC driver required |
| Connection | `dbGetInfo(con)` | Returns db.version, dbname, username, host, port from WebSocket session |
| Driver | `dbGetInfo(exasol())` | Returns driver info without RODBC.version |
| Query execution | `dbGetQuery(con, "SELECT 1")` | Returns data.frame with value 1 |
| Metadata | `dbListTables(con)` | Returns character vector of table names |
| Transaction management | `dbBegin(con); dbExecute(con, "CREATE TABLE t(x INT)"); dbCommit(con)` | Table created and committed |
| Bulk read | `exa.readData(con, "SELECT * FROM t")` | Returns data.frame via HTTP channel |
| Bulk write | `exa.writeData(con, data.frame(x=1:10), "t")` | Writes 10 rows via HTTP channel |

### Checklist

| Step | Command | Expected |
|------|---------|----------|
| Build | `devtools::build()` | Exit 0 |
| Install | `pak::local_install(dependencies = TRUE)` | Exit 0, no RODBC dependency |
| Test | `devtools::test()` | 0 failures |
| C++ tests | `cmake -B build tests/cpp && cmake --build build && ./build/<test-binary>` | 0 failures |
| R CMD check | `R CMD build . && R CMD check exasol_*.tar.gz` | 0 errors, 0 warnings |
| Docker integration | `tests/run_test_within_docker.sh` | 0 failures |
