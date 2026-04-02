# Tasks: change-odbc-to-websocket

## Phase 1: C++ Libraries (Group A)
- [x] 1.1 Select and integrate C++ WebSocket library into build system
- [x] 1.2 Select and integrate C++ JSON library into build system

## Phase 2: C++ WebSocket Core (Group B)
- [x] 2.1 Implement C++ WebSocket transport layer (connect ws/wss, send/receive JSON, close)
- [x] 2.2 Implement C++ authentication flow (login, RSA public key, PKCS#1 v1.5, Base64)
- [x] 2.3 Implement C++ command layer (execute, fetch, disconnect, setAttributes, getAttributes, closeResultSet)
- [x] 2.4 Implement C++ error handling (parse JSON error responses with sqlCode and text)

## Phase 3: R Bridge + Refactor (Group C)
- [x] 3.1 Create R-to-C++ bridge functions (.Call) for WebSocket operations
- [x] 3.2 Refactor EXAConnection S4 class (remove RODBC superclass, add WebSocket handle slot)
- [x] 3.3 Refactor EXADriver S4 class (remove ODBC driver path logic)

## Phase 4: R Layer Rewrite (Group D)
- [x] 4.1 Rewrite .EXANewConnection (WebSocket connect + login)
- [x] 4.2 Rewrite query execution (WebSocket execute instead of sqlQuery)
- [x] 4.3 Rewrite transaction management (WebSocket execute + setAttributes)
- [x] 4.4 Rewrite table operations (WebSocket execute instead of sqlQuery)
- [x] 4.5 Replace OdbcQueryExecutor in bulk transfer path
- [x] 4.6 Update EXADBI-info.R (remove RODBC.version, use login response)

## Phase 5: Cleanup (Group E)
- [x] 5.1 Remove RODBC from DESCRIPTION Imports and NAMESPACE
- [x] 5.2 Remove dead ODBC code (headers, sources, bridge code)
- [x] 5.3 Update configure.ac for WebSocket and JSON libraries

## Phase 6: Testing (Group F)
- [x] 6.1 Write integration tests for WebSocket protocol scenarios
- [x] 6.2 Write integration tests for changed connection, query, transaction, metadata, bulk transfer
- [x] 6.3 Update documentation and examples

## Phase 7: Rcpp Migration (Group G)
- [x] 7.1 Add Rcpp to DESCRIPTION (LinkingTo, Imports) and NAMESPACE (importFrom/useDynLib)
- [x] 7.2 Convert WebSocket bridge (r_websocket_bridge.cpp) to Rcpp: XPtr<WsSession>, Rcpp::List returns, Rcpp::stop errors, [[Rcpp::export]]
- [x] 7.3 Convert bulk transfer wrappers (exasol.c) to Rcpp
- [x] 7.4 Run Rcpp::compileAttributes() to generate RcppExports.cpp/.R
- [x] 7.5 Update all R .Call() invocations to use Rcpp-generated wrappers
- [x] 7.6 Remove manual R_CallMethodDef registration; clean up exasol.c
- [x] 7.7 Update Makevars/Makevars.win/Makevars.ucrt for Rcpp include paths

## Phase 8: Verification
- [x] 8.1 Run build (R CMD INSTALL)
- [x] 8.2 Run tests (devtools::test()) with EXAHOST
- [x] 8.3 Run R CMD check (0 errors, 3 warnings, 5 notes — all pre-existing or cosmetic)
- [x] 8.4 Scenario coverage audit
- [x] 8.5 Code review (14 findings, 3 critical fixed)
