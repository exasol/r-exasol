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
#' @param silent Logical, if FALSE no log messages will be exhibited.
#' @return a logical indicating success.
#' @seealso \code{\link[DBI:dbCommit]{DBI::dbCommit()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbCommit", signature("EXAConnection"),
          function(conn, silent = FALSE) {
            switch(as.character(odbcEndTran(conn,commit = TRUE)),
              "-1" = {
                stop(paste0("Commit failed:\n",odbcGetErrMsg(conn)));return(FALSE)
              },
              "0" = {
                if (!silent) message("Transaction committed.");return(TRUE)
              },
            {
              print("Commit failed.")
              stop(odbcGetErrMsg(conn))
              return(FALSE)
            })
          })

#' @title dbRollback
#'
#' @describeIn dbRollback Rolls the current DB transaction back.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @param conn An EXAConnection object
#' @return a logical indicating success.
#' @seealso \code{\link[DBI:dbRollback]{DBI::dbRollback()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbRollback", signature("EXAConnection"),
          function(conn) {
            switch(as.character(odbcEndTran(conn,commit = FALSE)),
              "-1" = {
                stop(paste0("Rollback failed:\n", odbcGetErrMsg(conn)))
                return(FALSE)
              },
              "0" = {
                message("Transaction rolled back.")
                return(TRUE)
              },
            {
              print("Rollback failed.")
              stop(odbcGetErrMsg(conn))
              return(FALSE) # this line gets never executed
            })
          })

#' @title dbBegin
#' @describeIn dbBegin Starts a DB transaction. In EXASOL, it disables autocommit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @param conn An EXAConnection object
#' @return a logical indicating success.
#' @seealso \code{\link[DBI:dbBegin]{DBI::dbBegin()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbBegin", signature("EXAConnection"),
          function(conn) {
            odbcSetAutoCommit(conn, autoCommit = FALSE)
            #message("Transaction started.")
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
#' @return a logical indicating success.
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
#' @param commit Logical. on TRUE the transaction is commmitted, otherwise rolled back.
#' @param silent Logical. on FALSE, it shows logs in db actions.
#' @return a logical indicating success.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod("dbEnd", signature("EXAConnection"),
          function(conn,commit = TRUE, silent = FALSE) {
            ifelse(commit, dbCommit(conn, silent = silent), dbRollback(conn))
            odbcSetAutoCommit(conn, autoCommit = conn@autocom_default)
            # message("Transaction completed.")
            return(TRUE)
          })
