library(exasol)

# Connect to Exasol DB with an ODBC DSN
exaconn <- dbConnect(
  drv     = "exa",                  # EXAdriver object
  exahost = "192.168.56.103:8563",  # IP of database cluster
  uid     = "sys",                  # Username
  pwd     = "exasol")               # Password
