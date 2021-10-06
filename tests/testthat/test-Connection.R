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
  exaconn <- get_connection(encryption = "N", sslcertificate = "ABC", uselegacyencryption = "N")
  expect_equal(exaconn@db_user, "sys")
  sslcert <- .parse_odbc_value(exaconn@init_connection_string, "SSLCERTIFICATE=[\\w]+?;", 16)
  expect_equal(sslcert, "ABC")


  uselegacyenc_key_value <- regmatches(exaconn@init_connection_string, gregexpr("USELEGACYENCRYPTION=N$", exaconn@init_connection_string,perl = TRUE))[[1]]
  uselegacyenc <- substr(uselegacyenc_key_value, 21, nchar(uselegacyenc_key_value))
  expect_equal(uselegacyenc, "N")
  dbDisconnect(exaconn)
})

}