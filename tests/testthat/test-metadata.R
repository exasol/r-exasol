# Integration tests for metadata retrieval via WebSocket.
# These tests require a running Exasol database.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")
sslcert <- Sys.getenv("EXASSLCERTIFICATE", "SSL_VERIFY_NONE")

test_that("dbGetInfo on driver has no RODBC.version", {
  drv <- exasol_driver(silent = TRUE)
  info <- dbGetInfo(drv)
  expect_true(is.list(info))
  expect_null(info$RODBC.version)
  expect_true("driver.version" %in% names(info))
  expect_true("DBI.version" %in% names(info))
})

test_that("dbGetInfo on connection returns correct info", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer(dbDisconnect(con))

  info <- dbGetInfo(con)
  expect_true(is.list(info))
  expect_true("db.version" %in% names(info))
  expect_true("dbname" %in% names(info))
  expect_true("username" %in% names(info))
  expect_true("host" %in% names(info))
  expect_true("port" %in% names(info))
  expect_true(nchar(info$db.version) > 0)
  expect_equal(info$username, uid)
})

test_that("dbListTables returns table names", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer(dbDisconnect(con))

  tables <- dbListTables(con)
  expect_true(is.character(tables))
  # There should always be system tables
  expect_true(length(tables) > 0)
})

test_that("dbListFields returns column names", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  withr::defer({
    tryCatch(dbGetQuery(con, "DROP SCHEMA IF EXISTS TEST_METADATA_FIELDS CASCADE"), error = function(e) NULL)
    dbDisconnect(con)
  })

  dbGetQuery(con, "CREATE SCHEMA IF NOT EXISTS TEST_METADATA_FIELDS")
  dbGetQuery(con, "CREATE OR REPLACE TABLE TEST_METADATA_FIELDS.FIELD_TEST (ID INT, NAME VARCHAR(100), VAL DOUBLE)")

  fields <- dbListFields(con, "TEST_METADATA_FIELDS.FIELD_TEST")
  expect_true(is.character(fields))
  expect_equal(length(fields), 3)
  expect_true("ID" %in% fields)
  expect_true("NAME" %in% fields)
  expect_true("VAL" %in% fields)
})
