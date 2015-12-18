context("Writing data to EXASOL DB")

# Added by Viliam Simko
test_that("Testing error cases", {
  expect_error(exa.writeData(NULL, NULL, NULL),
               regexp = "argument is not an open RODBC channel")

  expect_error(exa.writeData(NULL, NULL, NULL, server = "localhost:0"),
               regexp = "Could not connect")
})
