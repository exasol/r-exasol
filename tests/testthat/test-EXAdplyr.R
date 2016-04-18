
# testdata:

con <- dbConnect("exa", dsn = "exasolution-uo2214lv1_64", schema = "PUB864")

df <- data.frame(city=c("München", "Stuttgart", "Kopenhagen", "New York", "Madrid"),
                 country=c("Germany", "Germany", "Schweden", "USA", "Spanien"), stringsAsFactors = FALSE)

dbWriteTable(con, name="PUB864.CITIES", df)



# EXAdplyr-tests:

#con <- src_exasol(host = "192.168.137.10", port=8563, user = "sys", password = "exasol", schema = "PUB864"); con
con <- src_exasol(dsn = "exasolution-uo2214lv1_64", schema = "PUB864"); con
src_desc(con)


tbls <- db_list_tables(con); tbls
bool <- db_has_table(con, "PUB864.CITIES"); bool

t <- tbl(con, "CITIES")
# explain(t)

df <- data.frame(a = c(1,2,3), b = c(4,5,6)); t <- copy_to(con, df, "PUB864.TEST_COPY_TO")



# cleanup:

dbGetQuery(con$con, "DROP TABLE PUB864.TEST_COPY_TO")

