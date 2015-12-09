context("Reading data from EXASOL")

# Added by Viliam Simko
test_that("Testing error cases", {
  expect_error(exa.readData(NULL, NULL),
               regexp = "argument is not an open RODBC channel")

  expect_error(exa.readData(NULL, NULL, server = "localhost:0"),
               regexp = "Could not connect")
})

# Added by Viliam Simko
test_that("Expecting that EXASOL VM runs locally", {
  skip_on_travis()
  skip_on_appveyor()
  skip_on_cran()

  # we expect this DSN to be configured in odbc.ini
  C <- odbcConnect("exasolution")
  expect_equal(exa.readData(C, "select * from DUAL"),
               data.frame(DUMMY = NA))

  tables <- exa.readData(C, "SELECT * FROM EXA_ALL_TABLES")
  expect_true(is.data.frame(tables))

  odbcClose(C)
})
