# Integration tests for WebSocket-based query execution.
# These tests require a running Exasol database.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")
sslcert <- Sys.getenv("EXASSLCERTIFICATE", "SSL_VERIFY_NONE")

test_that("non-SELECT executes via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer({
    tryCatch(dbGetQuery(con, "DROP SCHEMA IF EXISTS TEST_QUERY_NS CASCADE"), error = function(e) NULL)
    dbDisconnect(con)
  })

  # A DDL statement (non-SELECT)
  rs <- dbSendQuery(con, "CREATE SCHEMA IF NOT EXISTS TEST_QUERY_NS")
  expect_s4_class(rs, "EXAResult")
  dbClearResult(rs)
})

test_that("dbGetQuery returns data for SELECT", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer(dbDisconnect(con))

  result <- dbGetQuery(con, "SELECT 42 AS val")
  expect_true(is.data.frame(result))
  expect_equal(result$VAL[1], 42)
})

test_that("dbSendQuery + dbFetch works for SELECT", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer(dbDisconnect(con))

  rs <- dbSendQuery(con, "SELECT 1 AS x FROM DUAL")
  expect_s4_class(rs, "EXAResult")
  df <- dbFetch(rs)
  expect_true(is.data.frame(df))
  expect_equal(nrow(df), 1)
  dbClearResult(rs)
})
