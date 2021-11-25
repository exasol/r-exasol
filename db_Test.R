library(exasol)

drv <- "/home/thomas/Downloads/EXASOL_ODBC-6.2.7/lib/linux/x86_64/libexaodbc-uo2214lv2.so"
exa_drv <- exasol(driver = drv)
exaconn <- dbConnect(exa_drv, exahost="localhost:8888", uid = "sys", pwd = "exasol")

tbls <- dbListTables(exaconn)

for (iteration in 0:20) {
  message(paste0("Iteration:", iteration))
  for(tbl in tbls) {
    limit <- sample(80:100, 1)
    message(paste0("Reading table:", tbl, " with limit=", limit))
    exa.readData(exaconn, paste0("SELECT * FROM ", tbl, " LIMIT ", limit))
  }
}

dbDisconnect(exaconn)