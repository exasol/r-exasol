#' Execute a SQL query on an EXASolution database and read results fast.
#'
#' @description This function executes the given SQL query using a given RODBC
#' connection and returns the results as a data frame.
#'
#' The results are transfered via a proprietary high speed channel from the
#' database optimized for bulk transfer. The \code{EXPORT ... INTO CSV AT ...}
#' statement is used internally to transfer the results as a csv. This is
#' significantly faster than RODBC.
#'
#' On the R-side, the results are parsed per default via read.csv. You can also
#' use arbitrary readers, processing the incoming csv records according to your
#' needs.
#'
#' @param channel The RODBC connection channel, typically created via
#'   odbcConnect.
#' @param query A string with the SQL query to be executed on EXASolution.
#' @param encoding A string containing the DB encoding. By default "UTF-8".
#' There should be no need to change this as the DB will convert the result set before
#' sending if necessary.
#'
#' @param reader This is a function reading and processing the results coming
#'   from the database. The read.csv function is used per default.
#'
#'   The input for the reader is an R file connection object (as returned by the
#'   file function) containing the CSV input, as it is returned by the internal
#'   call of EXPORT TO CSV.
#'
#'   The following custom reader function would simply read and return the CSV
#'   formatted text:
#'
#'   \code{myReader <- function(fileConnection) {readLines(fileConnection)}}
#'
#' @param server This parameter is only relevant in rare cases where you want to
#'   customize the address of the data channel. Per default, the data channel
#'   uses the same host and port as the RODBC connection.
#'
#' @param ... Other parameters passed on to the reader (read.csv).
#'
#' @return The return value is the return value of the reader function. If the
#'   default read.csv is used, the result will be a dataframe as it is returned
#'   read.csv.
#'
#' @author EXASOL AG <support@@exasol.com>
#'
#' @example examples/readData.R
#' @export
exa.readData <- function(channel, query, encoding = 'UTF-8',
                         reader = function(x,..., enc = encoding) {
                           read.csv(x,..., stringsAsFactors = FALSE, encoding = enc,
                                    blank.lines.skip = FALSE, numerals="no.loss")
                         },
                         server = NA,...) {
  query <- as.character(query)

  try(.Call(C_asyncRODBCQueryFinish, 0))

  protocol <- ifelse(channel@encrypted, "https", "http")

  if (is.na(server)) {
    server <- odbcGetInfo(channel)[["Server_Name"]]
  }

  serverAddress <- strsplit(server, ":")[[1]]

  serverHost <- as.character(serverAddress[[1]])
  serverPort <- as.integer(serverAddress[[2]])

  .Call(C_asyncRODBCIOStart,serverHost, serverPort, protocol)

  proxyHost <- .Call(C_asyncRODBCProxyHost)
  proxyPort <- .Call(C_asyncRODBCProxyPort)
  query <- paste0("EXPORT (", query, ") INTO CSV AT '", protocol, "://",  proxyHost, ":",
                 proxyPort, "' FILE 'executeSQL.csv' ENCODING = '",encoding,"' BOOLEAN = 'TRUE/FALSE' WITH COLUMN NAMES")

  on.exit(.Call(C_asyncRODBCQueryFinish, 0))

  fd <- .Call(C_asyncRODBCQueryStart,
              attr(channel, "handle_ptr"), query, protocol, 0)

  res <- reader(fd,...)
  on.exit(NULL)
  .Call(C_asyncRODBCQueryFinish, 1)
  res
}
