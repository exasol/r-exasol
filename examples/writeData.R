\dontrun{

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
}
