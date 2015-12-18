#' Execute a SQL query on an EXASolution database and read results in a fast
#' way.
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
#' @param reader This is a function reading and processing the results coming
#'   from the database. The read.csv function is used per default.
#'
#'   The input for the reader is an R file connection object (as returned by the
#'   file function) containing the CSV input, as it is returned by the internall
#'   call of EXPORT TO LOCAL CSV.
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
#' @return The return value is the return value of the reader function. If the
#'   default read.csv is used, the result will be a dataframe as it is returned
#'   read.csv.
#'
#' @author EXASOL AG <support@@exasol.com>
#'
#' @example examples/readData.R
#' @export
exa.readData <- function(channel, query,
                         reader = function(...) {
                           read.csv(..., stringsAsFactors = FALSE,
                                    blank.lines.skip = FALSE)
                         },
                         server = NA) {
  slot <- 0
  query <- as.character(query)

  try(.Call(C_asyncRODBCQueryFinish, slot, 1))

  if (is.na(server)) {
    server <- odbcGetInfo(channel)[["Server_Name"]]
  }

  serverAddress <- strsplit(server, ":")[[1]]

  serverHost <- as.character(serverAddress[[1]])
  serverPort <- as.integer(serverAddress[[2]])

  .Call(C_asyncRODBCIOStart, slot, serverHost, serverPort)

  proxyHost <- .Call(C_asyncRODBCProxyHost, slot)
  proxyPort <- .Call(C_asyncRODBCProxyPort, slot)
  query <- paste("EXPORT (", query, ") INTO CSV AT 'http://",  proxyHost, ":",
                 proxyPort, "' FILE 'executeSQL.csv' WITH COLUMN NAMES",
                 sep = "")

  on.exit(.Call(C_asyncRODBCQueryFinish, slot, 1))

  fd <- .Call(C_asyncRODBCQueryStart, slot,
              attr(channel, "handle_ptr"), query, 0)

  res <- reader(fd)
  on.exit(NULL)
  .Call(C_asyncRODBCQueryFinish, slot, 0)
  res
}
