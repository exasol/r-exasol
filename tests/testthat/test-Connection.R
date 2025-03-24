if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {

get_connection <- function(...) {
  dbConnect("exa", exahost="exasol-test-database:8888", uid = "sys", pwd = "exasol", sslcertificate="/certificate/rootCA.crt", ...)
}

test_that("encryption_true", {
  exaconn <- get_connection(encryption = "Y")
  session_id <- exa.readData(exaconn, "SELECT CURRENT_SESSION")
  session_data <-
    exa.readData(exaconn, paste0("SELECT * FROM SYS.EXA_DBA_SESSIONS WHERE SESSION_ID=", session_id$CURRENT_SESSION))

  expect_true(session_data$ENCRYPTED)
  expect_true(exaconn@encrypted)
  dbDisconnect(exaconn)
})

test_that("connection_attributes", {
  exaconn <- get_connection()
  expect_true(exaconn@encrypted)
  sslcert <- grepl("SSLCERTIFICATE=/certificate/rootCA.crt;", exaconn@init_connection_string, fixed = TRUE)
  expect_true(sslcert)

  dbDisconnect(exaconn)
})

test_that("connection_no_attributes", {
  exaconn <- get_connection()
  expect_true(exaconn@encrypted)
  sslcert <- grepl("SSLCERTIFICATE=", exaconn@init_connection_string, fixed = TRUE)
  expect_false(sslcert)

  dbDisconnect(exaconn)
})

  test_that("cloned_connection", {
    exaconn <- get_connection()
    new_conn <- dbConnect(exaconn)
    sample_data <- exa.readData(new_conn, "SELECT 0 AS I FROM DUAL")
    expect_equal(sample_data$I[1], 0)

    dbDisconnect(new_conn)
    dbDisconnect(exaconn)
  })

}
