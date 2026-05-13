# Integration tests for bulk write (exa.writeData) via WebSocket.
# These tests require a running Exasol database.

skip_if_not(
  nchar(Sys.getenv("EXAHOST")) > 0,
  "EXAHOST environment variable not set"
)

host <- Sys.getenv("EXAHOST")
uid <- Sys.getenv("EXAUID", "sys")
pwd <- Sys.getenv("EXAPWD", "exasol")
sslcert <- Sys.getenv("EXASSLCERTIFICATE", "SSL_VERIFY_NONE")

test_that("exa.writeData triggers IMPORT via WebSocket", {
  con <- dbConnect("exa", exahost = host, uid = uid, pwd = pwd, sslcertificate = sslcert)
  test_schema <- "TEST_BULK_WRITE"
  withr::defer({
    tryCatch(dbGetQuery(con, paste0("DROP SCHEMA IF EXISTS ", test_schema, " CASCADE")), error = function(e) NULL)
    dbDisconnect(con)
  })

  dbGetQuery(con, paste0("CREATE SCHEMA IF NOT EXISTS ", test_schema))
  dbGetQuery(con, paste0("CREATE OR REPLACE TABLE ", test_schema, ".WRITE_TEST (ID INT, NAME VARCHAR(50))"))

  df <- data.frame(ID = 1:5, NAME = c("alpha", "beta", "gamma", "delta", "epsilon"))

  result <- exa.writeData(con, df, tableName = paste0(test_schema, ".WRITE_TEST"))
  expect_true(result)

  # Verify the data was written
  read_back <- exa.readData(con, paste0("SELECT * FROM ", test_schema, ".WRITE_TEST ORDER BY ID"))
  expect_equal(nrow(read_back), 5)
  expect_equal(read_back$ID, 1:5)
})
