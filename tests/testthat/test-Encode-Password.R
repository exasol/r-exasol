
test_that("encode_password_no_curly_brakets", {
  pwd <- "password"
  result <- .encode_password(pwd = pwd)
  expect_equal(result, "{password}")
})
