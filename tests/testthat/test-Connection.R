if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {

get_connection_with_certificate <- function(...) {
  dbConnect("exa", exahost="exasol-test-database:8888", uid = "sys", pwd = "exasol", encryption = "Y", ...)
}

get_connection_without_certificate <- function(...) {
  dbConnect("exa", exahost="exasol-test-database:8888", uid = "sys", pwd = "exasol", encryption = "Y", ...)
}

test_that("encryption_true", {
  exaconn <- get_connection_with_certificate()
  session_id <- exa.readData(exaconn, "SELECT CURRENT_SESSION")
  session_data <-
    exa.readData(exaconn, paste0("SELECT * FROM SYS.EXA_DBA_SESSIONS WHERE SESSION_ID=", session_id$CURRENT_SESSION))

  expect_true(session_data$ENCRYPTED)
  expect_true(exaconn@encrypted)
  dbDisconnect(exaconn)
})

test_that("connection_attributes", {
  exaconn <- get_connection_with_certificate()
  expect_true(exaconn@encrypted)
  expect_true(.Call(C_exaWsIsConnected, exaconn@ws_handle))
  dbDisconnect(exaconn)
})

test_that("connection_no_encryption", {
  # Test unencrypted connection if server allows it
  tryCatch({
    exaconn <- dbConnect("exa", exahost="exasol-test-database:8888", uid = "sys", pwd = "exasol", encryption = "N")
    expect_false(exaconn@encrypted)
    dbDisconnect(exaconn)
  }, error = function(e) {
    skip("Server may require TLS; skipping unencrypted test")
  })
})

  test_that("cloned_connection", {
    exaconn <- get_connection_with_certificate()
    new_conn <- dbConnect(exaconn)
    sample_data <- exa.readData(new_conn, "SELECT 0 AS I FROM DUAL")
    expect_equal(sample_data$I[1], 0)

    dbDisconnect(new_conn)
    dbDisconnect(exaconn)
  })

}
