#' @include EXADBI-connection.R
NULL

## Transaction related methods declared by DBI API.

#' @title dbCommit
#' @describeIn dbCommit Sends a commit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @param conn An EXAConnection object
#' @param silent A logical, if FALSE no log messages will be exhibited.
#' @return A logical indicating success.
#' @seealso \code{\link[DBI:dbCommit]{DBI::dbCommit()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbCommit", signature("EXAConnection"),
          function(conn, silent = FALSE) {
            exaWsExecute(conn@ws_handle, "COMMIT")
            if (!silent) message("Transaction committed.")
            return(TRUE)
          })

#' @title dbRollback
#'
#' @describeIn dbRollback Rolls the current DB transaction back.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @param conn An EXAConnection object
#' @return A logical indicating success.
#' @seealso \code{\link[DBI:dbRollback]{DBI::dbRollback()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbRollback", signature("EXAConnection"),
          function(conn) {
            exaWsExecute(conn@ws_handle, "ROLLBACK")
            message("Transaction rolled back.")
            return(TRUE)
          })

#' @title dbBegin
#' @describeIn dbBegin Starts a DB transaction. In EXASOL, it disables autocommit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @param conn An EXAConnection object
#' @return A logical indicating success.
#' @seealso \code{\link[DBI:dbBegin]{DBI::dbBegin()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbBegin", signature("EXAConnection"),
          function(conn) {
            exaWsSetAttributes(conn@ws_handle, '{"autocommit":false}')
            return(TRUE)
          })

#' @title dbEnd
#' Ends a DB transaction. In EXASOL, it commits and reinstates the connection's standard autocommit mode.
#' This is an EXASOL specific addition to the DBI interface and may not work with other RDBMS.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#' @family None-DBI-API
#'
#' @param conn An EXAConnection object
#' @param ... Additional parameters
#' @return A logical indicating success.
#' @export
#' @author EXASOL AG <opensource@exasol.com>
setGeneric(
  "dbEnd",
  def = function(conn,...)
    standardGeneric("dbEnd"),
  valueClass = "logical"
)

#' @describeIn dbEnd Ends a DB transaction.
#' @family EXADriver related objects
#' @family EXAConnection related objects
#'
#' @param conn An EXAConnection object
#' @param commit A logical, on TRUE the transaction is committed, otherwise rolled back.
#' @param silent A logical, on FALSE, it shows logs in db actions.
#' @return A logical indicating success.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbEnd", signature("EXAConnection"),
          function(conn, commit = TRUE, silent = FALSE) {
            ifelse(commit, dbCommit(conn, silent = silent), dbRollback(conn))
            autocommit_json <- ifelse(conn@autocom_default,
                                      '{"autocommit":true}',
                                      '{"autocommit":false}')
            exaWsSetAttributes(conn@ws_handle, autocommit_json)
            return(TRUE)
          })
