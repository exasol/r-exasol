# Plan: replace-ixwebsocket-with-beast

## Summary

Replace the vendored ixwebsocket C++ library (37 compiled source files) with Boost.Beast (header-only via the BH R package) for WebSocket communication, reducing build time, package size, and eliminating vendored code maintenance. Also replace ixwebsocket's Base64 utility with OpenSSL's EVP Base64 functions.

## Design

### Context

The current WebSocket transport uses ixwebsocket, a vendored C++ library comprising 37 `.cpp` files and ~50 headers in `src/r_exasol/external/ixwebsocket/`. This contributes 22 MB to the installed package size, increases compile time significantly, and triggers R CMD check NOTEs for `stderr`/`stdout` usage in vendored code. Boost.Beast is a header-only alternative available through the standard `BH` CRAN package (v1.90.0-1, Boost 1.90), already verified to compile with the project's toolchain.

- **Goals** — Remove vendored ixwebsocket; use Boost.Beast via BH for WebSocket transport; use OpenSSL for Base64 encoding; maintain identical behavior at the R layer; reduce installed package size
- **Non-Goals** — Changing the synchronous request/response pattern; modifying the R-level API or Rcpp bridge; switching to async I/O; fixing unrelated R CMD check warnings

### Decision

#### Architecture

```
Rcpp bridge (rcpp_bridge.cpp)
    │
    └── WebSocketClient (websocket_client.h/.cpp)
            │
            ├── Boost.Beast synchronous WebSocket stream
            │     ├── boost::beast::websocket::stream<tcp::socket>          (ws://)
            │     └── boost::beast::websocket::stream<ssl_stream<tcp::socket>>  (wss://)
            │
            ├── Boost.Asio (io_context, tcp::resolver, ssl::context)
            │
            └── OpenSSL (TLS via Asio SSL, Base64 via EVP)
```

The `WebSocketClient` class interface (`connect`, `sendAndReceive`, `sendOnly`, `close`, `isConnected`) remains unchanged. Only the implementation changes from ixwebsocket calls to Beast synchronous operations.

#### Patterns

| Pattern | Where | Why |
|---------|-------|-----|
| Synchronous Beast I/O | `WebSocketClient` | R is single-threaded; no benefit from async. Simpler code, matches existing blocking semantics |
| Type-erased stream | `WebSocketClient` | `std::variant` or pointer to handle both TLS and plain WebSocket streams behind a uniform interface |
| OpenSSL EVP Base64 | `exasol_auth.cpp` | Already linked; replaces ixwebsocket's `IXBase64.h` with zero new dependencies |

### Consequences

| Decision | Alternatives Considered | Rationale |
|----------|------------------------|-----------|
| Boost.Beast via BH | Keep ixwebsocket; standalone Asio | BH is a standard CRAN dependency, header-only (no vendored .cpp), maintained by Boost community |
| Synchronous I/O | Async with io_context | R process is single-threaded; async adds complexity with no benefit |
| OpenSSL EVP for Base64 | Beast detail::base64; vendored header | OpenSSL is already linked; EVP is public stable API |
| `std::variant` for TLS/plain | Virtual base class; template-only | Variant avoids heap allocation and virtual dispatch; Beast streams are not polymorphic |

## Features

| Feature | Status | Spec |
|---------|--------|------|
| WebSocket protocol | CHANGED | `connectivity/websocket-protocol/spec.md` |

## Dependencies

| Dependency | Purpose | Replaces |
|------------|---------|----------|
| BH (>= 1.84.0) | Boost.Beast + Boost.Asio headers | ixwebsocket vendored sources |
| OpenSSL (already linked) | TLS via Boost.Asio SSL + Base64 via EVP | ixwebsocket TLS + IXBase64.h |

## Migration

| Current | New |
|---------|-----|
| `ix::WebSocket` (async, callback-driven) | `boost::beast::websocket::stream` (synchronous) |
| `ix::initNetSystem()` / `ix::uninitNetSystem()` | Not needed (Asio handles this) |
| `ix::SocketTLSOptions` | `boost::asio::ssl::context` |
| `ix::WebSocket::setOnMessageCallback` | Direct `ws.read()` / `ws.write()` calls |
| `ix::WebSocket::start()` + polling/CV wait | `tcp::resolver::resolve()` + `ws.handshake()` |
| `ix::WebSocket::stop()` | `ws.close()` |
| `ix::WebSocket::sendText()` | `ws.write(boost::asio::buffer(msg))` |
| `ix::ReadyState::Open` | Check via `ws.is_open()` |
| `macaron::Base64::Encode()` (IXBase64.h) | `EVP_EncodeBlock()` (OpenSSL) |

## Implementation Tasks

1. Add `BH` to `LinkingTo` in DESCRIPTION
2. Rewrite `websocket_client.h` — replace ixwebsocket types with Beast types, use `std::variant` for TLS/plain streams
3. Rewrite `websocket_client.cpp` — implement `connect`, `sendAndReceive`, `sendOnly`, `close`, `isConnected` using Beast synchronous API
4. Replace Base64 in `exasol_auth.cpp` — swap `macaron::Base64::Encode` with OpenSSL `EVP_EncodeBlock`
5. Remove all vendored ixwebsocket files (`src/r_exasol/external/ixwebsocket/`)
6. Update `configure.ac` — remove ixwebsocket object files from `SOURCES_LIST`
7. Update `src/Makevars.win` and `src/Makevars.ucrt` — remove ixwebsocket object files
8. Update `src/sources.list` — remove ixwebsocket entries
9. Build and run integration tests against Exasol database

## Parallelization

| Parallel Group | Tasks |
|----------------|-------|
| Group A: Beast implementation | Task 2, Task 3, Task 4 |
| Group B: Cleanup | Task 5, Task 6, Task 7, Task 8 |
| Group C: Verification | Task 9 |

Sequential dependencies:
- Task 1 → Group A (Beast code needs BH headers)
- Group A → Group B (cleanup after new code works)
- Group B → Group C (verify final state)

## Dead Code Removal

| Type | Location | Reason |
|------|----------|--------|
| C++ sources (37 files) | `src/r_exasol/external/ixwebsocket/*.cpp` | Replaced by Boost.Beast |
| C++ headers (~50 files) | `src/r_exasol/external/ixwebsocket/*.h` | Replaced by Boost.Beast |
| Makevars entries | `configure.ac`, `src/sources.list`, `Makevars.win`, `Makevars.ucrt` | Object file references for removed sources |

## Verification

### Scenario Coverage

All existing scenarios from the `change-odbc-to-websocket` plan remain valid — this is an implementation swap, not a behavior change. The same 22 integration tests verify correctness.

| Scenario | Test Type | Test Location | Test Name |
|----------|-----------|---------------|-----------|
| Establish WebSocket connection | Integration | `tests/testthat/test-websocket-protocol.R` | WebSocket connection is established |
| Authenticate with credentials | Integration | `tests/testthat/test-websocket-protocol.R` | authentication succeeds with valid credentials |
| Handle authentication failure | Integration | `tests/testthat/test-websocket-protocol.R` | authentication fails with invalid credentials |
| Send command and receive response | Integration | `tests/testthat/test-websocket-protocol.R` | execute command returns response |
| Handle error response | Integration | `tests/testthat/test-websocket-protocol.R` | error response raises R error with sqlCode |
| Negotiate protocol version | Integration | `tests/testthat/test-websocket-protocol.R` | protocol version is negotiated |
| Close WebSocket connection | Integration | `tests/testthat/test-websocket-protocol.R` | disconnect closes WebSocket cleanly |
| Connect via hostname and credentials | Integration | `tests/testthat/test-connection.R` | dbConnect creates connection via WebSocket |
| Connect with driver name as string | Integration | `tests/testthat/test-connection.R` | dbConnect with string driver works without ODBC |
| Clone existing connection | Integration | `tests/testthat/test-connection.R` | dbConnect clones connection with new WebSocket session |
| Connect with encryption disabled | Integration | `tests/testthat/test-connection.R` | dbConnect with encryption=FALSE uses ws:// |
| Disconnect from database | Integration | `tests/testthat/test-connection.R` | dbDisconnect sends disconnect and closes WebSocket |
| Create driver without system ODBC | Integration | `tests/testthat/test-driver.R` | exasol() creates driver without ODBC |
| Execute non-SELECT statement | Integration | `tests/testthat/test-query.R` | non-SELECT executes via WebSocket |
| Get driver info | Integration | `tests/testthat/test-metadata.R` | dbGetInfo on driver has no RODBC.version |
| Begin a transaction | Integration | `tests/testthat/test-transaction.R` | dbBegin disables autocommit via WebSocket |
| Commit a transaction | Integration | `tests/testthat/test-transaction.R` | dbCommit sends COMMIT via WebSocket |
| Rollback a transaction | Integration | `tests/testthat/test-transaction.R` | dbRollback sends ROLLBACK via WebSocket |
| End transaction with commit | Integration | `tests/testthat/test-transaction.R` | dbEnd with commit restores autocommit |
| End transaction with rollback | Integration | `tests/testthat/test-transaction.R` | dbEnd with rollback restores autocommit |
| Read data via high-speed channel | Integration | `tests/testthat/test-bulk-read.R` | exa.readData triggers EXPORT via WebSocket |
| Write data via high-speed channel | Integration | `tests/testthat/test-bulk-write.R` | exa.writeData triggers IMPORT via WebSocket |

### Manual Testing

| Feature | Command | Expected Output |
|---------|---------|-----------------|
| WebSocket connect + query | `Rscript -e 'library(exasol); C <- dbConnect("exa", exahost="localhost:8888", uid="sys", pwd="exasol"); print(dbGetQuery(C, "SELECT 1")); dbDisconnect(C)'` | Returns data.frame with value 1, clean exit |
| TLS error message | `Rscript -e 'library(exasol); dbConnect("exa", exahost="localhost:8888", uid="sys", pwd="exasol", encryption="N")'` | Error message includes reason from Beast/Asio |
| Package size | `du -sh $(Rscript -e 'cat(system.file(package="exasol"))')` | Significantly smaller than 22 MB |

### Checklist

| Step | Command | Expected |
|------|---------|----------|
| Build | `R CMD INSTALL --no-multiarch --with-keep.source .` | Exit 0 |
| Test | `EXAHOST=localhost:8888 EXAUID=sys EXAPWD=exasol Rscript -e 'devtools::test()'` | 0 failures |
| R CMD check | `R CMD build . && R CMD check --no-manual --no-vignettes exasol_*.tar.gz` | 0 errors |
