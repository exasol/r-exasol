# Plan: replace-nlohmann-with-boost-json

## Summary

Replace the vendored nlohmann/json single-header library with Boost.JSON (available via the BH CRAN package) for JSON serialization and deserialization. This removes the last vendored C++ dependency, reduces installed package size, and consolidates on Boost as the sole external C++ library.

## Design

### Context

The project currently vendors `nlohmann/json.hpp` (~25k lines, single-header) in `src/r_exasol/external/nlohmann/`. It is used in 5 source files and 3 headers for building JSON commands, parsing server responses, and passing structured data between C++ and R. Boost.JSON is header-only and already available through the BH CRAN package (>= 1.84.0), which is already a dependency.

- **Goals** — Remove vendored nlohmann/json; use Boost.JSON for all JSON operations; maintain identical behavior at the R layer
- **Non-Goals** — Changing the JSON-over-WebSocket protocol; modifying the R-level API; adding new JSON features

### Decision

#### Architecture

```
Rcpp bridge (rcpp_bridge.cpp)
    │
    ├── ExasolCommands (exasol_commands.h/.cpp)
    │     └── boost::json::object / boost::json::value
    │
    ├── ExasolAuth (exasol_auth.h/.cpp)
    │     └── boost::json::object / boost::json::value
    │
    ├── parseResponse (exasol_error.h/.cpp)
    │     └── boost::json::parse / boost::json::value
    │
    └── jsonDataToRList (rcpp_bridge.cpp)
          └── boost::json::array / boost::json::value
```

#### API Migration

| nlohmann::json | Boost.JSON |
|---------------|------------|
| `json::parse(str)` | `boost::json::parse(str)` |
| `j.dump()` | `boost::json::serialize(v)` |
| `json obj; obj["k"] = v;` | `boost::json::object obj; obj["k"] = v;` |
| `json::array({v})` | `boost::json::array({v})` |
| `j.at("k")` | `obj.at("k")` |
| `j.value("k", default)` | Helper: `jsonValueOr(obj, "k", default)` |
| `j.get<std::string>()` | `v.as_string().c_str()` or `boost::json::value_to<std::string>(v)` |
| `j.get<double>()` | `v.as_double()` or `v.to_number<double>()` |
| `j.is_null()` | `v.is_null()` |
| `j.is_array()` | `v.is_array()` |
| `j.contains("k")` | `obj.contains("k")` |
| `j.find("k")` | `obj.find("k")` (returns iterator) |
| `j.size()` | `arr.size()` or `obj.size()` |
| `json::object()` | `boost::json::object{}` |
| `for (auto& el : j)` | `for (auto& el : arr)` or `for (auto& kv : obj)` |
| `json::parse_error` | `boost::system::system_error` |

#### Helper Function

nlohmann's `.value("key", default)` has no direct equivalent. Introduce a small inline helper:

```cpp
template<typename T>
T jsonValueOr(const boost::json::object& obj, const char* key, const T& defaultVal);
```

### Consequences

| Decision | Alternatives Considered | Rationale |
|----------|------------------------|-----------|
| Boost.JSON via BH | Keep nlohmann; RapidJSON | BH already linked; eliminates last vendored library; maintained by Boost |
| Inline helper for value-or | boost::json::value::if_contains + manual check | Keeps migration mechanical; single call site pattern |

## Features

| Feature | Status | Spec |
|---------|--------|------|
| WebSocket protocol | CHANGED | `connectivity/websocket-protocol/spec.md` |

## Dependencies

| Dependency | Purpose | Replaces |
|------------|---------|----------|
| BH (>= 1.84.0) | Boost.JSON headers | nlohmann/json vendored header |

## Migration

| Current | New |
|---------|-----|
| `#include <r_exasol/external/nlohmann/json.hpp>` | `#include <boost/json.hpp>` |
| `using json = nlohmann::json;` | `namespace json = boost::json;` |
| `json obj; obj["k"] = v;` | `json::object obj; obj["k"] = v;` |
| `json::parse(str)` | `json::parse(str)` |
| `j.dump()` | `json::serialize(v)` |
| `j.value("k", default)` | `jsonValueOr(obj, "k", default)` |
| `j.get<std::string>()` | `std::string(v.as_string())` |
| `j.get<double>()` | `v.to_number<double>()` |
| `j.find("k")` → iterator | `obj.find("k")` → iterator |
| `statusIt->get<std::string>()` | `std::string(statusIt->value().as_string())` |
| `json::parse_error` | `boost::system::system_error` |

## Implementation Tasks

1. Rewrite `exasol_error.h/.cpp` — replace nlohmann types with Boost.JSON; update `parseResponse()`
2. Rewrite `exasol_commands.h/.cpp` — replace `nlohmann::json` in `ExecuteResult` struct and all command methods
3. Rewrite `exasol_auth.cpp` — replace JSON construction and parsing in `login()`
4. Rewrite `rcpp_bridge.cpp` — replace `jsonDataToRList()` helper and all JSON usage in bridge functions
5. Rewrite `tests/test_websocket.cpp` — update test JSON construction
6. Remove vendored nlohmann files (`src/r_exasol/external/nlohmann/`)
7. Update `exasol_error.h` — remove nlohmann include, forward-declare or include Boost.JSON

## Parallelization

| Parallel Group | Tasks |
|----------------|-------|
| Group A: Core rewrite | Task 1 (error), Task 2 (commands), Task 3 (auth) |
| Group B: Bridge + tests | Task 4 (rcpp_bridge), Task 5 (tests) |
| Group C: Cleanup | Task 6 (remove nlohmann), Task 7 (header cleanup) |

Sequential dependencies:
- Group A must complete before Group B (bridge depends on updated types)
- Group B must complete before Group C (cleanup after all code migrated)

## Dead Code Removal

| Type | Location | Reason |
|------|----------|--------|
| C++ header (~25k lines) | `src/r_exasol/external/nlohmann/json.hpp` | Replaced by Boost.JSON |

## Verification

### Scenario Coverage

All existing integration tests remain valid — this is a serialization library swap, not a behavior change.

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
| JSON error handling | `Rscript -e 'library(exasol); C <- dbConnect("exa", exahost="localhost:8888", uid="sys", pwd="exasol"); tryCatch(dbGetQuery(C, "SELECT FROM"), error=function(e) cat("Error caught:", e$message, "\n")); dbDisconnect(C)'` | Error message includes Exasol sqlCode |

### Checklist

| Step | Command | Expected |
|------|---------|----------|
| Build | `R CMD INSTALL --no-multiarch --with-keep.source .` | Exit 0 |
| Test | `EXAHOST=localhost:8888 EXAUID=sys EXAPWD=exasol Rscript -e 'devtools::test()'` | 0 failures |
| R CMD check | `R CMD build . && R CMD check --no-manual --no-vignettes exasol_*.tar.gz` | 0 errors |
