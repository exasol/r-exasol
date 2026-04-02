# Integration tests for WebSocket protocol layer.
# These tests require a running Exasol database.
# Set EXAHOST, EXAUID, EXAPWD environment variables to run.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")
schema <- Sys.getenv("EXASCHEMA", "TEST_SCHEMA")

test_that("WebSocket connection is established", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  expect_s4_class(con, "EXAConnection")
  expect_true(exaWsIsConnected(con@ws_handle))
})

test_that("authentication succeeds with valid credentials", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  expect_true(con@session_id > 0L)
})

test_that("authentication fails with invalid credentials", {
  expect_error(
    dbConnect("exa", exahost = host, uid = "invalid_user_xyz", pwd = "wrong_password"),
    class = "error"
  )
})

test_that("execute command returns response", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  result <- exaWsExecute(con@ws_handle, "SELECT 1 AS x")
  expect_true(is.list(result))
  expect_equal(result$numResults, 1)
})

test_that("error response raises R error for invalid SQL", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  expect_error(exaWsExecute(con@ws_handle, "SELECT FROM nonexistent_table_xyz"))
})

test_that("protocol version is negotiated and session ID is valid", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))
  # Session ID is a positive whole number (numeric, not integer, because Exasol uses 64-bit IDs)
  expect_true(is.numeric(con@session_id))
  expect_true(con@session_id > 0)
  expect_equal(con@session_id, floor(con@session_id))
})

test_that("disconnect closes WebSocket cleanly", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  dbDisconnect(con)
  expect_false(exaWsIsConnected(con@ws_handle))
})
