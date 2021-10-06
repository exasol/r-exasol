if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {

get_connection <- function(...) {
  dbConnect("exa", exahost="localhost:8888", uid = "sys", pwd = "exasol", ...)
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

test_that("encryption_false", {
  exaconn <- get_connection(encryption = "N")
  session_id <- exa.readData(exaconn, "SELECT CURRENT_SESSION")
  session_data <-
    exa.readData(exaconn, paste0("SELECT * FROM SYS.EXA_DBA_SESSIONS WHERE SESSION_ID=", session_id$CURRENT_SESSION))

  expect_false(session_data$ENCRYPTED)
  expect_false(exaconn@encrypted)
  dbDisconnect(exaconn)
})


test_that("connection_attributes", {
  exaconn <- get_connection(encryption = "Y", sslcertificate = "ABC", uselegacyencryption = "N")
  expect_true(exaconn@encrypted)
  sslcert <- grepl("SSLCERTIFICATE=ABC;", exaconn@init_connection_string, fixed = TRUE)
  expect_true(sslcert)

  uselegacyenc <- grepl("USELEGACYENCRYPTION=N", exaconn@init_connection_string, fixed = TRUE)
  expect_true(uselegacyenc)
  dbDisconnect(exaconn)
})

test_that("connection_no_attributes", {
  exaconn <- get_connection()
  expect_false(exaconn@encrypted)
  sslcert <- grepl("SSLCERTIFICATE=", exaconn@init_connection_string, fixed = TRUE)
  expect_false(sslcert)

  uselegacyenc <- grepl("USELEGACYENCRYPTION=", exaconn@init_connection_string, fixed = TRUE)
  expect_false(uselegacyenc)
  dbDisconnect(exaconn)
})

}