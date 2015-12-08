context("R scripts as UDF in EXASOL")

# Added by Viliam Simko
test_that("Creating a script without ODBC connection", {
  testscript <- exa.createScript(
    NULL, mockOnly = TRUE, # no connection inside unit-tests
    "test.mymean", # script name
    function(data) {
      data$next_row(NA); # read all values from this group into a single vector
      data$emit(data$groupid[[1]], mean(data$val))
    },
    inArgs = c( "groupid INT", "val DOUBLE" ),
    outArgs = c( "groupid INT", "mean DOUBLE" )
  )

  expect_true(is.function(testscript))
  sql <- testscript("groupid", "val", table = "test.twogroups" ,
                    groupBy = "groupid", returnSQL = TRUE)
  expect_equal(sql,
    paste("(SELECT * FROM (SELECT test.mymean(groupid, val)",
          "FROM test.twogroups GROUP BY groupid))") )
})
