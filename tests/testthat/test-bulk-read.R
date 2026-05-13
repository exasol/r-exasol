# Integration tests for bulk read (exa.readData) via WebSocket.
# These tests require a running Exasol database.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")
sslcert <- Sys.getenv("EXASSLCERTIFICATE", "SSL_VERIFY_NONE")

test_that("exa.readData triggers EXPORT via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer(dbDisconnect(con))

  # exa.readData uses the high-speed HTTP channel triggered by a WebSocket EXPORT command
  result <- exa.readData(con, "SELECT 1 AS x, 'hello' AS y FROM DUAL")
  expect_true(is.data.frame(result))
  expect_equal(nrow(result), 1)
  expect_equal(result$X[1], 1)
  expect_equal(result$Y[1], "hello")
})

test_that("exa.readData reads multiple rows", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer(dbDisconnect(con))

  result <- exa.readData(con, "SELECT COLUMN_VALUE AS val FROM VALUES 1, 2, 3, 4, 5 AS t(COLUMN_VALUE)")
  expect_true(is.data.frame(result))
  expect_equal(nrow(result), 5)
})
