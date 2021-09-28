## EXAObject declaration and definition of generic object related DBI API methods.

#' EXAObject class.
#'
#' The virtual object constituting a basis to all other EXA DBI Objects.
#' @seealso \code{\link{DBIObject-class}}
#' @family DBI classes
#'
#' @docType class
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setClass("EXAObject", contains = c("DBIObject", "VIRTUAL"))


#' @title dbIsValid
#' @describeIn dbIsValid Checks if an EXAObject is still valid.
#'
#' @param dbObj An object that inherits EXAObject.
#' @return A logical indicating if the connection still works.
#' @seealso \code{\link[DBI:dbIsValid]{DBI::dbIsValid()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbIsValid", signature("EXAObject"),
  definition = function(dbObj) {
    return(TRUE) # TODO
  }
)