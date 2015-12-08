context("R scripts as UDF in EXASOL")

# Added by Viliam Simko
test_that("Creating a script without ODBC connection", {

  for (outType in c(EMITS, SET, SCALAR, RETURNS)) {
    testscript <- exa.createScript(
      NULL, mockOnly = TRUE, # no connection inside unit-tests
      "test.mymean",
      function(data) {},
      initCode = function() {},
      cleanCode = function() {},
      inArgs = c( "groupid INT", "val DOUBLE" ),
      outArgs = c( "groupid INT", "mean DOUBLE" ),
      outType = outType
    )

    expect_true(is.function(testscript))
    sql <- testscript("groupid", "val", table = "test.twogroups" ,
                      groupBy = "groupid", returnSQL = TRUE)
    expect_equal(sql,
                 paste("(SELECT * FROM (SELECT test.mymean(groupid, val)",
                       "FROM test.twogroups GROUP BY groupid))") )
  }
})

# Added by Viliam Simko
test_that("No output arguments should fail", {
  expect_error(exa.createScript(
    NULL, "test.mymean"
  ), regexp = "No output arguments")
})
