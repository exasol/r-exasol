#' @docType package
#' @name exasol-package
#' @aliases exasol
#' @useDynLib exasol, .registration = TRUE, .fixes = "C_"
#' @exportPattern ^[[:alpha:]]+
#' @import RODBC
#' @import DBI
#'
#' @title R Interface & SDK for the EXASOL Database
#'
#' @description The EXASOL R Package offers interface functionality such as connecting to, querying
#' and writing into an EXASOL Database (version 5 onwards). It is optimised for massively parallel
#' reading & writing from and to a multinode cluster. Implemented are DBI compliant methods for database
#' access, querying and modiifcation. The package integrates with EXASOL's InDB R UDF capabilities, which
#' allows to deploy and execute R code dynamically from an R application running on a client.
#'
#' EXASOL is an InMemory RDBMS that runs in a MPP cluster (shared-nothing) environment. Leading the TPC-H
#' benckmark, it is considered the fastest analytical data warehouse available. The community edition
#' can be downloaded for free on \url{https://docs.exasol.com/get_started/communityedition.htm}.
#'
#'
#'
#' @section Package index:
#'
#'\subsection{DBI: General}{
#'
#'\describe{
#' \item{\code{\link{EXAObject-class}}}{A virtual meta Object for all other EXASOL DBI objects.}
#' \item{\code{\link{dbGetInfo}}}{}
#' \item{\code{\link{summary}}}{}
#' \item{\code{\link{dbIsValid}}}{Checks if an EXAObject is still valid.}
#' \item{\code{\link{dbDataType}}}{Determine the EXASOL data type of an object.}
#' }
#'}
#'
#'\subsection{DBI: Driver-related}{
#'
#'\describe{
#' \item{\code{\link{EXADriver-class}}}{An interface driver object to the EXASOL Database.}
#' \item{\code{\link{dbGetInfo}}}{}
#' \item{\code{\link{summary}}}{}
#' \item{\code{\link{dbDriver}}}{Load database drivers.}
#' \item{\code{\link{dbUnloadDriver}}}{Unload a driver.}
#' \item{\code{\link{dbConnect}}}{Creates a connection to an EXASOL Database.}
#' }
#'}
#'
#' \subsection{DBI: Connection-related}{
#' \describe{
#' \item{\code{\link{EXAConnection-class}}}{An object holding a connection to an EXASOL Database.}
#' \item{\code{\link{dbGetInfo}}}{}
#' \item{\code{\link{summary}}}{}
#' \item{\code{\link{dbDisconnect}}}{Disconnects the connection.}
#' \item{\code{\link{dbSendQuery}}}{Sends an SQL statment to an EXASOL DB, prepares for result fetching.}
#' \item{\code{\link{dbGetQuery}}}{Executes the query, fetches and returns the entire result set.}
#' \item{\code{\link{dbGetException}}}{}
#' \item{\code{\link{dbListResults}}}{}
#' \item{\code{\link{dbListFields}}}{}
#' \item{\code{\link{dbListTables}}}{}
#' \item{\code{\link{dbReadTable}}}{Reads a DB table.}
#' \item{\code{\link{dbWriteTable}}}{Writes a data.frame into a table. If the table does not exist, it is created.}
#' \item{\code{\link{dbExistsTable}}}{Checks if a table exists in an EXASOL DB.}
#' \item{\code{\link{dbRemoveTable}}}{Removes a table.}
#' \item{\code{\link{dbBegin}}}{Starts a DB transaction.}
#' \item{\code{\link{dbEnd}}}{Ends a DB transaction.}
#' \item{\code{\link{dbCommit}}}{Sends a commit.}
#' \item{\code{\link{dbRollback}}}{Rolls the current DB transaction back.}
#' }}
#'
#' \subsection{DBI: Result set-related}{
#' \describe{
#' \item{\code{\link{EXAResult-class}}}{An object that is associated with a result set in an EXASOL Database.}
#' \item{\code{\link{dbGetInfo}}}{}
#' \item{\code{\link{summary}}}{}
#' \item{\code{\link{dbFetch}}}{Fetches a subset of an result set.}
#' \item{\code{\link{dbClearResult}}}{Frees all resources associated with an \code{EXAResult}.}
#' \item{\code{\link{dbColumnInfo}}}{}
#' \item{\code{\link{dbGetStatement}}}{}
#' \item{\code{\link{dbHasCompleted}}}{}
#' \item{\code{\link{dbGetRowsAffected}}}{}
#' \item{\code{\link{dbGetRowCount}}}{}
#' }}
#'
#' \subsection{Low-level methods}{
#' \describe{
#' \item{\code{\link{exa.readData}}}{Execute a SQL query on an EXASolution database and read results fast.}
#' \item{\code{\link{exa.writeData}}}{Write a data.frame into an EXASOL table fast.}
#' \item{\code{\link{exa.createScript}}}{Deploys an R function as an UDF in the EXASolution database.}
#' \item{\code{\link{dbCurrentSchema}}}{Fetches and outputs the current schema from an EXASOL DB.}
#' }}
#'
#'
#' @author EXASOL AG & Community
#'
#' Maintainer: \packageMaintainer{exasol}
#'
#' @references
#'
#' \enumerate{
#' \item The development version of the package is available on \url{https://github.com/EXASOL/r-exasol}
#' \item Bugs and improvements may be noted on \url{https://github.com/EXASOL/r-exasol/issues}
#' \item Downloads & manuals related to the EXASOL Database are at \url{https://docs.exasol.com/get_started/communityedition.htm}
#'}
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
"C_asyncRODBCProxyHost"

#' TODO comment
"C_asyncRODBCProxyPort"

#' TODO comment
"C_asyncRODBCQueryFinish"

#' TODO comment
"C_asyncEnableTracing"

#' TODO comment
"C_run_testthat_tests"

.onAttach <- function(libname, pkgname) {
  # show startup message
  message <- paste("EXASOL SDK", utils::packageVersion("exasol"), "loaded.")
  packageStartupMessage(message, appendLF = TRUE)
}
