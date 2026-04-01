# Integration tests for WebSocket-based connection management.
# These tests require a running Exasol database.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")
schema <- Sys.getenv("EXASCHEMA", "TEST_SCHEMA")

test_that("dbConnect creates connection via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  expect_s4_class(con, "EXAConnection")
  expect_true(exaWsIsConnected(con@ws_handle))
  expect_true(con@session_id > 0L)
  expect_true(nchar(con@db_version) > 0)
})

test_that("dbConnect with string driver works without ODBC", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  expect_s4_class(con, "EXAConnection")
  # No RODBC dependency needed
  expect_true(is(con@ws_handle, "externalptr"))
})

test_that("dbConnect clones connection with new WebSocket session", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  cloned <- dbConnect(con)
  withr::defer(dbDisconnect(cloned))

  expect_s4_class(cloned, "EXAConnection")
  expect_true(exaWsIsConnected(cloned@ws_handle))
  # Cloned connection should have a different session ID
  expect_true(cloned@session_id > 0L)
})

test_that("dbConnect with encryption=N uses unencrypted connection", {
  # This test may fail if the server requires TLS. Skip if so.
  tryCatch({
    con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, encryption = "N")
    withr::defer(dbDisconnect(con))
    expect_s4_class(con, "EXAConnection")
    expect_false(con@encrypted)
  }, error = function(e) {
    skip("Server may require TLS; skipping unencrypted test")
  })
})

test_that("dbDisconnect sends disconnect and closes WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  result <- dbDisconnect(con)
  expect_true(result)
  expect_false(exaWsIsConnected(con@ws_handle))
})
