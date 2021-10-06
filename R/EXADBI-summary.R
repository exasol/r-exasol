#' @include EXADBI-object.R
#' @include EXADBI-connection.R
#' @include EXADBI-result.R
#' @include EXADBI-driver.R
NULL

#' @export summary
NULL

## Summary methods of the DBI API.
## All methods forward to DBI::summary("DBIObject")

#' @title summary
#' @describeIn summary Summary info for EXASOL object
#' @param object Any EXA object.
#' @param ... Further parameters passed to summary().
#' @returns A summary string of the EXAObject
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "summary", "EXAObject",
  definition = function(object, ...) {
    NextMethod(generic = "summary", object, ...)
  }
)

#' @describeIn summary Summary info for EXASOL EXAResult
#' @param object Any EXAResult object.
#' @param ... Further parameters passed to summary().
#' @returns A summary string of the EXAResult
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "summary", "EXAResult",
  definition = function(object, ...) {
    NextMethod(generic = "summary", object,...)
  }
)

#' @describeIn summary Summary info for EXASOL EXADriver
#' @param object Any EXADriver object.
#' @param ... Further parameters passed to summary().
#' @returns A summary string of the EXADriver
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "summary", "EXADriver",
  definition = function(object, ...)
    NextMethod(generic = "summary", object,...)
)

#' @describeIn summary Info for EXASOL EXAConnection.
#' @param object EXAConnection object for which the summary info will be returned.
#' @param ... Further parameters passed to summary().
#' @returns A summary string of the EXADriver
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "summary", "EXAConnection",
  definition = function(object, ...)
    NextMethod(generic = "summary", object,...)
)
