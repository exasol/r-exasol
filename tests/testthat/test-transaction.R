# Integration tests for WebSocket-based transaction management.
# These tests require a running Exasol database.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")

test_that("dbBegin disables autocommit via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))

  result <- dbBegin(con)
  expect_true(result)
  # Restore autocommit
  dbEnd(con, commit = FALSE)
})

test_that("dbCommit sends COMMIT via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))

  dbBegin(con)
  result <- dbCommit(con, silent = TRUE)
  expect_true(result)
})

test_that("dbRollback sends ROLLBACK via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))

  dbBegin(con)
  result <- dbRollback(con)
  expect_true(result)
})

test_that("dbEnd with commit restores autocommit", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))

  dbBegin(con)
  result <- dbEnd(con, commit = TRUE, silent = TRUE)
  expect_true(result)
  # After dbEnd, autocommit should be restored to the connection default
})

test_that("dbEnd with rollback restores autocommit", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd)
  withr::defer(dbDisconnect(con))

  dbBegin(con)
  result <- dbEnd(con, commit = FALSE)
  expect_true(result)
  # After dbEnd with rollback, autocommit should be restored
})
