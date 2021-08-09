exaconn <- dbConnect(drv = "exa", exahost="localhost:8888", uid = "sys", pwd = "exasol")
exa.readData(exaconn, "SELECT * FROM TEST.ENGINETABLE", protocol = "https")