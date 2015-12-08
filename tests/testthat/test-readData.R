context("Reading data from EXASOL")

# Added by Viliam Simko
test_that("Testing error cases", {
  expect_error(exa.readData(NULL, NULL),
               regexp = "argument is not an open RODBC channel")

  expect_error(exa.readData(NULL, NULL, server = "localhost:0"),
               regexp = "Could not connect")
})
