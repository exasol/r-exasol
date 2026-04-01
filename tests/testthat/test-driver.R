# Integration tests for EXADriver without ODBC dependency.

test_that("exasol_driver() creates driver without ODBC", {
  drv <- exasol_driver(silent = TRUE)
  expect_s4_class(drv, "EXADriver")
  expect_true(is(drv, "DBIDriver"))
})

test_that("exa alias creates driver without ODBC", {
  drv <- exa(silent = TRUE)
  expect_s4_class(drv, "EXADriver")
})

test_that("dbDriver with 'exa' string creates EXADriver", {
  drv <- dbDriver("exa")
  expect_s4_class(drv, "EXADriver")
})
