\dontrun{

require(RODBC)
require(exasol)

# Connect via RODBC with configured DSN
C <- odbcConnect("exasolution")

# Read results
tables <- exa.readData(C, "SELECT * FROM EXA_ALL_TABLES")

# Work with the data frame returned (examples)
print(nrow(tables))      # print number of rows
print(colnames(tables))  # print names of columns
print(tables[1,])        # print first row
print(tables$TABLE_NAME[1])  # print first value of specified column

}
