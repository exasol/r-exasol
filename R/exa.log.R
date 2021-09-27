#' @include EXADBI-connection.R

##Log related functions

#' @title EXAConnectionLog
#' Shows the EXASOL ODBC connection log.
#' @param con An EXAConnection object
#' @param view logical If true, the log is shown in R View, otherwise, a data.frame is returned.
#' @return a data.frame, if View is FALSE.
#' @export
EXAConnectionLog <- function (con, view = TRUE) {
  logItems <- function(l) {
    time <-
      regmatches(l,gregexpr("^\\d\\d:\\d\\d:\\d\\d.\\d\\d\\d",l, perl = TRUE))
    time <- substr(time,0,nchar(time))
    num <-
      substr(regmatches(l,gregexpr("^.{12}\\t\\d+?\\t",l, perl = TRUE)),14,18)
    text <- sub("^.{12}\\t\\d+?\\t", "",l)
    data.frame(time = time,num = num, text = text)
  }

  lstr <-
    regmatches(
      con@init_connection_string,gregexpr("EXALOGFILE=\\S+?;",con@init_connection_string,perl =
        TRUE)
    )[[1]]
  lstr <- substring(lstr, 12, nchar(lstr) - 1)
  con <- file(lstr)
  df <- logItems(readLines(con))
  df$time <- strptime(df$time, format = "%T")
  if (view)
    View(df)
  else
    return(df)
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
    .Call(C_asyncEnableTracing, tracefile)
  }
}