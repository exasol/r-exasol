# Tasks: replace-ixwebsocket-with-beast

## Phase 1: Prerequisites
- [x] 1.1 Add BH to LinkingTo in DESCRIPTION

## Phase 2: Implementation (Group A - Beast implementation)
- [x] 2.1 Rewrite websocket_client.h — replace ixwebsocket types with Beast types, use std::variant for TLS/plain streams
- [x] 2.2 Rewrite websocket_client.cpp — implement connect, sendAndReceive, sendOnly, close, isConnected using Beast synchronous API
- [x] 2.3 Replace Base64 in exasol_auth.cpp — swap macaron::Base64::Encode with OpenSSL EVP_EncodeBlock

## Phase 2: Implementation (Group B - Cleanup)
- [x] 2.4 Remove all vendored ixwebsocket files (src/r_exasol/external/ixwebsocket/)
- [x] 2.5 Update configure.ac — remove ixwebsocket CPPFLAGS defines, upgrade C++14 to C++17
- [x] 2.6 Update src/Makevars.win — remove ixwebsocket object files and defines, C++17
- [x] 2.7 Update src/Makevars.ucrt — remove ixwebsocket object files and defines, C++17
- [x] 2.8 Update src/sources.list — remove ixwebsocket entries

## Phase 3: Verification
- [x] 3.1 Build package (R CMD INSTALL) — exit 0
- [x] 3.2 Run integration tests — 63 passed, 1 pre-existing failure, 3 skipped

## Phase 4: Code Review
- [x] 4.1 Review all changed files for quality — fixed findings
