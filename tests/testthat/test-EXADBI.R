## EXADBI tests
##
## added 2015 by Marcel Boldt
##
##
##



# Driver & Connection
#

# EXAdrv <- dbDriver("exa") # should execute the exasol method and return an EXADriver object class(EXAdrv)
#
#
# con <- dbConnect(EXAdrv,dsn="EXASolo") # create connection with an EXADriver object
# exa.readData(con, "select current_session")
# dbDisconnect(con)
#
# con <- dbConnect("exa",dsn="EXASolo") # create connection from character string + DSN
# con
# exa.readData(con, "select current_session")
#
#
#
# clone_con <- dbConnect(con,AUTOCOMMIT="N") # clone the connection
# exa.readData(clone_con, "select current_session")
# print(clone_con@init_connection_string)
# print(clone_con@connection.string)
#
# dbDisconnect(clone_con)
#
#
#
#
# # Querying tests
# #
#
# res <- dbSendQuery(con, "select * from CITIES.CITIES")
# res
# cities <- dbFetch(res, 20) # look for encoding problems (München)
#
# res$is_complete
#
# dbClearResult(res)
#
# dbGetQuery(con, "select * from cities.cities") # also look for encoding problems (München)
#
# dbReadTable(con, "cities.cities")
#
# dbExistsTable(con, "CITIES.CITIES")
#
# # Writing tests
# #
#
# if(dbExistsTable(con, "test.cities")) {dbRemoveTable(con, "test.cities")}
# dbWriteTable(con, "test.cities", cities) # first time, neither table nor schema does exist
# dbWriteTable(con, "test.cities", cities) # 2nd time, table does exist
# dbWriteTable(con, "test.cities", cities,overwrite=TRUE) # 3nd time, table does exist
# dbWriteTable(con, "test.new_tbl", cities) # 4rd time, schema exists, but no table
#
# dbRemoveTable(con, "test.new_tbl")
# # dbRemoveTable(con, "test.new_tbl",cascade=TRUE)
# dbDisconnect(con)
#
