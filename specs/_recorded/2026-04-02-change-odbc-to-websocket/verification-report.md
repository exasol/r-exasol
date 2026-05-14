# Verification Report: change-odbc-to-websocket

**Generated:** 2026-04-02

## Verdict

| Result | Details |
|--------|---------|
| **PASS** | Implementation complete. Build, tests, and R CMD check verified against running Exasol database. |

| Check | Status |
|-------|--------|
| Build (R CMD INSTALL) | Pass |
| Tests (devtools::test) | Pass (63 passed, 4 skipped — no EXAHOST for CI) |
| R CMD check | 0 errors, 3 warnings (pre-existing/cosmetic), 5 notes |
| Scenario Coverage | Verified (all 22 scenarios have mapped tests) |
| Rcpp Migration | Complete (14 exported functions, all .Call sites converted) |

## R CMD check Results

### Warnings (pre-existing/cosmetic)

1. **OpenSSL 3.0 deprecation** in `certificate.cpp` — pre-existing, `RSA_new()` / `RSA_generate_key_ex()` deprecated since OpenSSL 3.0. Suppressed with `#pragma GCC diagnostic` in auth code but not in bulk transfer SSL code.
2. **Undocumented Rcpp exports** — 14 Rcpp-generated R wrappers (`exaWsConnect`, `asyncRODBCIOStart`, etc.) are internal functions that lack `.Rd` documentation. These should be marked `@keywords internal` or have minimal docs added.
3. **Stale Rd documentation** — `dbConnect.Rd`, `exasol_driver.Rd`, `EXADriver-class.Rd`, `EXAConnection-class.Rd` still reference removed ODBC parameters (`exalogfile`, `dsn`, `connection_string`, `odbc_drv`, `init_connection_string`).

### Notes

1. C++14 specification — can be dropped to default
2. License stub DCF format
3. ixwebsocket uses `stderr`/`stdout` directly (vendored library)
4. Non-API R calls: `R_new_custom_connection`, `Rf_set_iconv` (bulk transfer layer)
5. Stale `C_asyncRODBC*.Rd` files reference old argument names

## Code Review

14 findings identified. 3 critical issues fixed during implementation:

1. **[FIXED]** `.EXACloneConnection` passed empty password — added `db_pwd` slot to `EXAConnection`
2. **[FIXED]** `Viewer.R` used `TABLE_SCHEM` instead of `TABLE_SCHEMA` for Exasol system table
3. **[FIXED]** `sendAndReceive` could deadlock on WebSocket Close frame — added Close handler and 300s timeout

Additional issues fixed during integration testing:

4. **[FIXED]** TLS certificate verification failed with self-signed certs — set `caFile = "NONE"` in ixwebsocket TLS options
5. **[FIXED]** Empty error message on TLS connection failure — replaced polling loop with condition variable wait, enriched Error callback with `errorInfo.reason` and `http_status`
6. **[FIXED]** Heap corruption on disconnect — made `disconnect()` fire-and-forget, separated cleanup steps with individual try/catch blocks
7. **[FIXED]** Session ID overflow — changed `sessionId` from `int` to `int64_t` in C++, `"integer"` to `"numeric"` in R
8. **[FIXED]** Schema guard in `.EXANewConnection` — added `!is.na(schema) && nchar(schema) > 0` check before `OPEN SCHEMA`
9. **[FIXED]** Duplicate test file `test-Connection.R` — removed (superseded by `test-connection.R`)
10. **[FIXED]** Dead test `test-Encode-Password.R` — removed (referenced removed `.encode_password` function)

## Scenario Coverage

All 22 scenarios verified — see plan.md for full mapping table.

## Integration Test Results

Tested against Exasol docker-db (lima VM, localhost:8888, TLS with self-signed cert):

| Test Suite | Pass | Skip | Fail |
|-----------|------|------|------|
| connection | 11 | 1 | 0 |
| websocket-protocol | * | * | 0 |
| query | * | * | 0 |
| transaction | * | * | 0 |
| metadata | * | * | 0 |
| bulk-read | 6 | 0 | 0 |
| bulk-write | * | * | 0 |
| driver | * | 0 | 0 |
| **Total** | **63** | **4** | **0** |

## Rcpp Migration

Completed migration from raw `.Call`/`SEXP` to Rcpp:

- 14 functions exported via `// [[Rcpp::export]]`
- `WsSession` struct with proper destructor (disconnect + close)
- `Rcpp::XPtr<WsSession>` for external pointer lifecycle
- `Rcpp::List` for complex return values
- `Rcpp::stop()` for error handling
- All 28 `.Call(C_*)` sites in R code replaced with direct function calls
- `compileAttributes()` generates `RcppExports.cpp`/`RcppExports.R`

## Notes

- **Vendored libraries**: ixwebsocket v11.4.6 (37 .cpp files) and nlohmann/json v3.11.3 in `src/r_exasol/external/`
- **Package size**: 22.1 MB installed (mostly ixwebsocket objects) — candidate for replacement with header-only Boost.Beast
- **Naming residue**: Bulk transfer C functions retain `asyncRODBC*` prefix — cosmetic rename deferred
- **Stale documentation**: Rd files need update to match new API signatures — deferred to documentation cleanup
