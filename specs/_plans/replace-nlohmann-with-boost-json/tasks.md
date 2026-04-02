# Tasks: replace-nlohmann-with-boost-json

## Phase 2: Implementation (Group A — Core rewrite)
- [x] 2.1 Rewrite exasol_error.h/.cpp — replace nlohmann types with Boost.JSON; update parseResponse()
- [x] 2.2 Rewrite exasol_commands.h/.cpp — replace nlohmann::json in ExecuteResult struct and all command methods
- [x] 2.3 Rewrite exasol_auth.cpp — replace JSON construction and parsing in login()

## Phase 2: Implementation (Group B — Bridge + tests)
- [x] 2.4 Rewrite rcpp_bridge.cpp — replace jsonDataToRList() helper and all JSON usage in bridge functions
- [x] 2.5 Rewrite tests/test_websocket.cpp — update test JSON construction

## Phase 2: Implementation (Group C — Cleanup)
- [x] 2.6 Remove vendored nlohmann files (src/r_exasol/external/nlohmann/)
- [x] 2.7 Update any remaining includes — ensure no nlohmann references remain

## Phase 3: Verification
- [x] 3.1 Build succeeds
- [x] 3.2 Run test suite — 0 failures (65 passed, 3 skipped)
- [x] 3.3 R CMD check — 0 errors (3 WARNINGs + 4 NOTEs are pre-existing)
