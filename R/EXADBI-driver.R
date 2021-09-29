#' @include EXADBI-object.R
NULL

## EXADriver declaration and definition of driver instantiation functions.

#' An interface driver object to the EXASOL Database.
#'
#' @seealso \code{\link{DBIDriver-class}}
#' @family DBI classes
#' @family EXADriver related objects
#' @slot odbc_drv A string containing the path to the EXASOL ODBC driver file.
#' @export
#' @author EXASOL AG <opensource@exasol.com>
setClass("EXADriver",
         contains = c("DBIDriver", "EXAObject"),
         slots = c(odbc_drv = "character")
)

#' Instantiates an EXADriver object.
#' @family EXADriver related objects
#' @param driver The path to an ODBC driver file. If missing, the driver installed on the system is used.
#' If "SYSTEM": the EXASOL ODBC driver installed on the system is used immediately.
#' Alternatively a path to an ODBC driver library can be provided.
#' @param silent If TRUE, no message is print.
#' @return An EXADriver object.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
exasol_driver <- function(driver = NULL, silent = FALSE) {

  #TODO: determine driver file according to OS
  # path.package("exasol")

  if(missing(driver) || is.null(driver) || driver =="SYSTEM") {
    if(!silent) print("Using the system driver...")
    driver <- "{EXASolution Driver}"
  } else {
    if (!silent) print(paste("Using the driver at", driver))
  }

  if (!silent) print("EXASOL driver loaded")
  new("EXADriver", odbc_drv = driver)
}

#' @describeIn exasol_driver An alias for \code{\link{exasol_driver}}
exa <- exasol_driver
