#' @docType package
#' @name exasol-package
#' @aliases exasol
#' @useDynLib exasol, .registration = TRUE, .fixes = "C_"
#' @exportPattern ^[[:alpha:]]+
#' @import RODBC
#'
#' @title EXASolution R Package
#'
#' @description The EXASolution R Package offers functionality to interact with
#'   the EXASolution database out of R programs. It is developed as a wrapper
#'   around ORDBC and extends ORDBC in two main aspects:
#'
#' \enumerate{
#'   \item It offers fast data transfer between EXASolution and R, multiple
#'   times faster than RODBC. This is achieved by using a proprietary transfer
#'   channel which is optimized for batch loading.
#'   Please read the R help of \code{exa.readData()} and \code{exa.writeData()} for details.
#'
#'   \item It makes it convenient to run parts of your R code in parallel on the
#'   EXASolution database, using EXASolution R UDF scripts behind the scenes.
#'   For example you can define an R function and execute it in parallel on
#'   different groups of data in an EXASolution table.
#'   Please read the R help of \code{exa.createScript()} function for details.
#' }
#'
#' The help is available directly in R via:
#' \itemize{
#'   \item \code{help(exa.readData)}
#'   \item \code{help(exa.writeData)}
#'   \item \code{help(exa.createScript)}
#' }
#'
#' @author EXASOL AG <support@@exasol.com>
#'
#' @keywords sql
#' @keywords distributed
#' @keywords in-memory
NULL

#' SET input type of UDF script will call the function once for each group
SET <- "SET"

#' SCALAR input type of UDF script will call the function once for each record.
SCALAR <- "SCALAR"

#' EMITS output type of UDF script -- function emits any number of values.
EMITS <- "EMITS"

#' RETURNS output type of UDF script -- function emits just a single value.
RETURNS <- "RETURNS"

#' All input types of UDF scripts
ALLOWED_UDF_IN_TYPES <- c(SET, SCALAR)

#' All output types of UDF scripts
ALLOWED_UDF_OUT_TYPES <- c(EMITS, RETURNS)

#' TODO comment
"C_asyncRODBCQueryStart"

#' TODO comment
"C_asyncRODBCIOStart"

#' TODO comment
"C_asyncRODBCIsDone"

#' TODO comment
"C_asyncRODBCMax"

#' TODO comment
"C_asyncRODBCProxyHost"

#' TODO comment
"C_asyncRODBCProxyPort"

#' TODO comment
"C_asyncRODBCQueryCheck"

#' TODO comment
"C_asyncRODBCQueryFinish"

.onAttach <- function(libname, pkgname) {
  # show startup message
  message <- paste("EXASOL RODBC", utils::packageVersion("exasol"), "loaded.")
  packageStartupMessage(message, appendLF = TRUE)
}




# require(RODBC); require(exasol)
# cnx <- odbcDriverConnect("Driver=/var/Executables/bc/install/ok7500-e8/lib/libexaodbc-uo2214.so;UID=sys;PWD=exasol;EXAHOST=cmw72;EXAPORT=8563")
# sqlQuery(cnx, "OPEN SCHEMA TEST")
# require(RODBC); require(exasol); cnx <- odbcDriverConnect("Driver=/var/Executables/bc/install/ok7500-e8/lib/libexaodbc-uo2214.so;UID=sys;PWD=exasol;EXAHOST=cmw67;EXAPORT=8563"); sqlQuery(cnx, "OPEN SCHEMA TEST")

#cnx <- odbcDriverConnect("Driver=/var/Executables/bc/install/ok7500-e8/lib/libexaodbc-uo2214.so;UID=sys;PWD=exasol;EXAHOST=cmw72;EXAPORT=8563")
#testScript <- exa.createScript(cnx, testScript,
#env = list(a = 1, b1 = 2, b2 = 2, b3 = 2, b4 = 2, b5 = 2, b6 = 2, b7 = 2, b8 = 2, b9 = 2, ba = 2, bo = 2, be = 2, bu = 2, bi = 2, bd = 2, bh = 2, bt = 2, bn = 2),
#inArgs = { INT(a) },
#outArgs = { INT(b); INT(c) },
#outputAddress = c('192.168.5.61', 3000),
#initCode = {
#  require(RODBC); require(data.table)
#  print(paste("initialize", exa$meta$vm_id));
#},
#func = function(data) {
#  print("begin group")
#  data$next_row(NA);
#  data$emit(data$a, data$a + 3);
#  print("end group")
#})
#
#
#res <- testScript(1, test)
#res <- exa.readData(cnx, 'select testScript(1) from test')
#exa.writeData(cnx, test)
#
#res <- sqlQuery(cnx, 'select testScript(1) from test')

# print(testScript(int_index, table = enginetable, groupBy = mod(int_index, 4), returnSQL = TRUE))
# print(summary(testScript(int_index, table = enginetable, groupBy = mod(int_index, 4))))

# require(RODBC)
# require(exasol); cnx <- odbcDriverConnect("DSN=EXA"); sqlQuery(cnx, "open schema test"); exa.readData(cnx, "select * from cat")
