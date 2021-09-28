#' @include EXADBI-object.R
#' @include EXADBI-connection.R
NULL

## Declaration of class EXAResult and result related DBI methods.

#' An object that is associated with a result set in an EXASOL Database.
#'
#' The result set is persisted
#' in a DB table, which is dropped when the object is deleted in R (on garbage collection), or manually by
#' `dbClearResult()'. R versions before 3.3.0 do not finalise objects on R exit, so if R is quit with an
#' active EXAResult object, the table may stay in the DB.
#'
#' @seealso \code{\link{DBIResult-class}}
#' @family DBI classes
#' @family EXAResult related objects
#'
#' @field connection An EXAConnection object.
#' @field statement A string containing the SQL query statement.
#' @field rows_fetched An int reflecting the rows already fetched from the DB.
#' @field rows_affected An int reflecting the length of the dataset in the DB.
#' @field is_complete A logical indcating if the result set has been entirely fetched.
#' @field with_output A logical indicating whether the query produced a result set.
#' @field profile A data.frame containing profile information on the query.
#' @field columns A data.frame containing column metadata.
#' @field temp_result_tbl A string reflecting the name of the (temporary) table that holds the result set.
#' @field query_sent_time A POSIXct timestamp indicating when the query was sent to the DB.
#' @field errors A character vector containing errors.
#' @field default_fetch_rec An int reflecting the default fetch size.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
EXAResult <- setRefClass(
  "EXAResult",
  fields = c(
    connection = "EXAConnection",
    statement = "character",
    rows_fetched = "numeric",
    rows_affected = "numeric",
    is_complete = "logical",
    with_output = "logical",
    profile = "data.frame",
    columns = "data.frame",
    temp_result_tbl = "character",
    query_sent_time = "POSIXct",
    errors = "character",
    default_fetch_rec = "numeric"
  ),
  contains = c("DBIResult", "EXAObject"),
  methods = list(
    refreshMetaData = function(x) {
      "Refreshes the object's metadata."
      print("TODO: not yet implemented.")
    },
    addRowsFetched = function(x) {
      "Add an int (the length of a newly fetched result set) to rows_fetched."
      rows_fetched <<- rows_fetched + as.numeric(x)
    },

    #         close = function(commit=TRUE) {                         # dbClearResult is in row 880
    #           "Frees up all resources, in particular drops the temporary table in the DB."
    #
    #             if (!dbIsValid(.self)) {
    #               warning("Connection seems exipired.\n    ...it's gone...")
    #               return(FALSE)
    #             }
    #
    #             if(odbcEndTran(connection, commit)==0) {
    #                 if(commit) message("Changes commited.")
    #             } else stop("Commit failed. Changes NOT commited. Closing aborted.")
    #           message("Closing connection...")
    #             res <- try(odbcClose(connection),silent=TRUE)
    #             if(res==1) {
    #                 message("Connection closed.")
    #                 return(TRUE)
    #             } else if(res==0) {
    #                 warning("Closing not successful.")
    #                 return(FALSE)
    #             } else {
    #                     warning(res)
    #                     return(FALSE)
    #                 }
    #         },

    finalize = function(...) {
      #close()
      if(dbClearResult(.self)) message("Table dropped in EXASOL DB.") else warning("Table not dropped in EXASOL DB")
      message("EXAResult object disposed.")
    }
  )
)

#' @title dbHasCompleted
#' @describeIn dbHasCompleted Returns if the operation has completed.
#' @param res EXAResult object to query on.
#' @seealso \code{\link[DBI:dbHasCompleted]{DBI::dbHasCompleted()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbHasCompleted", signature("EXAResult"),
  definition = function(res) {
    if(res$temp_result_tbl == "CLEARED") stop("GetInfo: Result set cleared.");
    return(res$is_complete);
  }
)

#' @title dbGetRowsAffected
#' @describeIn dbGetRowsAffected Get rows affected in result.
#' @seealso \code{\link[DBI:dbGetRowsAffected]{DBI::dbGetRowsAffected()}}
#' @param res Result from query.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbGetRowsAffected", signature("EXAResult"), function(res) {
  return(res$rows_affected)
})
