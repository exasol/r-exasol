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

## Phase 7: Verification
- [ ] 7.1 Run build (devtools::build())
- [ ] 7.2 Run tests (devtools::test())
- [ ] 7.3 Run R CMD check
- [ ] 7.4 Run C++ tests
- [x] 7.5 Scenario coverage audit
- [x] 7.6 Code review (14 findings, 3 critical fixed)
