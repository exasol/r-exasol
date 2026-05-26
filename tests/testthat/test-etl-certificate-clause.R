test_that("etl certificate clause is enabled for 2025.1.0 and later", {
  expect_equal(exasol:::.exa_etl_certificate_clause("2025.1.0"), " IGNORE CERTIFICATE")
  expect_equal(exasol:::.exa_etl_certificate_clause("2025.1.0 build 12"), " IGNORE CERTIFICATE")
  expect_equal(exasol:::.exa_etl_certificate_clause("2026.2.3"), " IGNORE CERTIFICATE")
})

test_that("etl certificate clause is omitted for older versions", {
  expect_equal(exasol:::.exa_etl_certificate_clause("7.1.17"), "")
  expect_equal(exasol:::.exa_etl_certificate_clause("2024.12.99"), "")
})

test_that("etl certificate clause fails closed for unknown versions", {
  expect_error(
    exasol:::.exa_etl_certificate_clause("unknown"),
    "Cannot determine compatible IMPORT/EXPORT certificate syntax"
  )
  expect_error(
    exasol:::.exa_etl_certificate_clause(""),
    "missing DBMS_Ver metadata"
  )
})
