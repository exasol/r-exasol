context("R scripts as UDF in EXASOL")

# Added by Viliam Simko
test_that("Creating a script without ODBC connection", {

  # should fail when outType has wrong value
  expect_error(
    exa.createScript(NULL, "test.mymean", outType = "WRONG TYPE"),
    regexp = "should be one of")

  # should fail when inType has wrong value
  expect_error(
    exa.createScript(NULL, "test.mymean", inType = "WRONG TYPE"),
    regexp = "should be one of")

  # now checking all allowed UDF output types
  for (out_type in ALLOWED_UDF_OUT_TYPES) {
    testscript <- exa.createScript(
      NULL, mockOnly = TRUE, # no connection inside unit-tests
      "test.mymean",
      function(data) {},
      initCode = function() {},
      cleanCode = function() {},
      inArgs = c( "groupid INT", "val DOUBLE" ),
      outArgs = c( "groupid INT", "mean DOUBLE" ),
      outType = out_type
    )

    expect_true(is.function(testscript))
    sql <- testscript("groupid", "val", table = "test.twogroups" ,
                      groupBy = "groupid", where = "true",
                      returnSQL = TRUE)
    expect_equal(sql,
                 paste("(SELECT * FROM (SELECT test.mymean(groupid, val)",
                       "FROM test.twogroups WHERE true GROUP BY groupid))") )
  }
})

# Added by Viliam Simko
test_that("No output arguments should fail", {
  expect_error(exa.createScript(
    NULL, "test.mymean"
  ), regexp = "No output arguments")
})
