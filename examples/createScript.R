\dontrun{

# This example creates a simple SET-EMITS script and executes
# it the table footable.
require(RODBC)
require(exasol)

# Connect via RODBC with configured DSN
C <- odbcConnect("exasolution")

# Generate example data frame with two groups
# of random values with different means.
valsMean0  <- rnorm(10, 0)
valsMean50 <- rnorm(10, 50)
twogroups <- data.frame(group = rep(1:2, each = 10),
                        value = c(valsMean0, valsMean50))

# Write example data to a table
odbcQuery(C, "CREATE SCHEMA test")
odbcQuery(C, "CREATE TABLE test.twogroups (groupid INT, val DOUBLE)")
exa.writeData(C, twogroups, tableName = "test.twogroups")

# Create the R function as an UDF R script in the database
# In our case it computes the mean for each group.
testscript <- exa.createScript(
  C,
  "test.mymean",
  function(data) {
    data$next_row(NA); # read all values from this group into a single vector
    data$emit(data$groupid[[1]], mean(data$val))
  },
  inArgs = c( "groupid INT", "val DOUBLE" ),
  outArgs = c( "groupid INT", "mean DOUBLE" ) )

# Run the function, grouping by the groupid column
# and aggregating on the "val" column. This returns
# two values which are close to the means of the two groups.
testscript("groupid", "val", table = "test.twogroups" , groupBy = "groupid")
}
