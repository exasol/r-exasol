#' @include EXADBI-object.R
NULL

## EXADriver declaration and definition of driver instantiation functions.

#' An interface driver object to the EXASOL Database.
#'
#' @seealso \code{\link{DBIDriver-class}}
#' @family DBI classes
#' @family EXADriver related objects
#' @export
#' @author EXASOL AG <opensource@exasol.com>
setClass("EXADriver",
         contains = c("DBIDriver", "EXAObject")
)

#' Instantiates an EXADriver object.
#' @family EXADriver related objects
#' @param silent If TRUE, no message is printed.
#' @return An EXADriver object.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
exasol_driver <- function(silent = FALSE) {
  if (!silent) message("EXASOL driver loaded")
  new("EXADriver")
}

#' @describeIn exasol_driver An alias for \code{\link{exasol_driver}}
exa <- exasol_driver
