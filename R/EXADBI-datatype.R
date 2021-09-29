#' @include EXADBI-object.R
NULL

## EXADatatype conversion

#' Generic for conversion to an EXASOL data type.
#' @keywords internal
setGeneric("EXADataType", function(x)
  standardGeneric("EXADataType"))

#' @describeIn EXADataType Generic method for conversion of a dataframe to an EXASOL data type.
#' @keywords internal
setMethod("EXADataType", "data.frame", function(x) {
  vapply(x, EXADataType, FUN.VALUE = character(1), USE.NAMES = FALSE)
})

#' @describeIn EXADataType Generic method for conversion of an integer to an EXASOL "INT".
#' @keywords internal
setMethod("EXADataType", "integer",  function(x)
  "INT")

#' @describeIn EXADataType Generic method for conversion of all other numerics to an EXASOL "DECIMAL(36,15)".
#' @keywords internal
setMethod("EXADataType", "numeric",  function(x)
  "DECIMAL(36,15)")

#' @describeIn EXADataType Generic method for conversion of a logical to an EXASOL "BOOLEAN".
#' @keywords internal
setMethod("EXADataType", "logical",  function(x)
  "BOOLEAN")

#' @describeIn EXADataType Generic method for conversion of a Date to an EXASOL "DATE".
#' @keywords internal
setMethod("EXADataType", "Date",     function(x)
  "DATE")

#' @describeIn EXADataType Generic method for conversion of a POSIXCT to an EXASOL "TIMESTAMP".
#' @keywords internal
setMethod("EXADataType", "POSIXct",  function(x)
  "TIMESTAMP")


# Helper function to create EXASOL SQL string definition.
#
# @keywords internal
.varchar <- function(x) {
  max_varchar_length = max(nchar(as.character(x)), na.rm=TRUE)
  if (is.infinite(max_varchar_length)) {
    paste0("CLOB")
  } else if (max_varchar_length == 0) {
    paste0("VARCHAR(1)")
  } else {
    paste0("VARCHAR(", max_varchar_length, ")")
  }
}

#' @describeIn EXADataType Generic method for conversion of a string to an EXASOL SQL string.
#' @keywords internal
setMethod("EXADataType", "character", .varchar)

#' @describeIn EXADataType Generic method for conversion of a factor to an EXASOL SQL string.
#' @keywords internal
setMethod("EXADataType", "factor",    .varchar)

#' @describeIn EXADataType Generic method for conversion of a list to an EXASOL SQL string.
#' @keywords internal
setMethod("EXADataType", "list", function(x) {
  vapply(x, EXADataType, FUN.VALUE = character(1), USE.NAMES = FALSE)
})

#' @describeIn EXADataType Generic method for conversion of a "raw" to an EXASOL SQL string.
#' @keywords internal
setMethod("EXADataType", "raw",  .varchar)

#' @describeIn EXADataType Generic method for conversion of "ANY" to an EXASOL SQL string.
#' @keywords internal
setMethod("EXADataType", "ANY", function(x) {
  warning(paste("Unrecognised datatype:", x, "Trying to convert to varchar."))
  .varchar(x)
})

#' @title dbDataType
#' @describeIn dbDataType Determines the EXASOL data type of an object.
#' @param dbObj A object inheriting from [EXADriver-class] or [EXAConnection-class].
#' @param obj An R object whose SQL type we want to determine.
#' @param ... Unused.
#' @seealso \code{\link{dbDataType,DBIObject-method}}
#' @aliases dbDataType,EXAObject-method
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbDataType", signature("EXAObject"),
          definition = function(dbObj, obj, ...) {
  if(any(class(obj) %in% c("factor", "data.frame")))
    EXADataType(obj)
  else
    EXADataType(unclass(obj))
})
