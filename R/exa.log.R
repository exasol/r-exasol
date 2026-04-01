#' @include EXADBI-connection.R

##Log related functions

#' @title EXAConnectionLog
#' Shows the EXASOL connection log.
#'
#' This function is no longer supported after the migration from ODBC to WebSocket.
#' It previously relied on the ODBC connection string to find the log file.
#' Use EXATrace to enable C-level tracing instead.
#'
#' @param con An EXAConnection object
#' @param view logical If true, the log is shown in R View, otherwise, a data.frame is returned.
#' @return a data.frame, if View is FALSE.
#' @export
EXAConnectionLog <- function (con, view = TRUE) {
  stop("EXAConnectionLog is no longer supported. Use EXATrace() for C-level tracing.")
}


#' @title EXATrace
#' Enables tracing for debugging purpose.
#' @param tracefile The output tracefile
#' @export
EXATrace <- function (tracefile) {
  tracefileAsString <- as.character(tracefile)
  if (tracefileAsString == "") {
    warning("Tracefile is empty.")
  } else {
    asyncEnableTracing(tracefile)
  }
}