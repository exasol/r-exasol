#' Write data frame from R to an EXASolution table.
#'
#' @description This function writes the given data frame to a database table.
#'
#' The data are transfered to the database via a proprietary transfer channel
#' which is optimized for high speed bulk transfer. To be more detailed, the
#' \code{IMPORT INTO ... FROM CSV AT ...} statement is used internally. On the
#' R-side, the data.frame is converted to csv format and written to a file
#' connection, streaming the data to the database. You can also use arbitrary
#' writers, processing the data frame according to your needs.
#'
#' @param channel The RODBC connection channel, typically created via odbcConnect.
#' @param data The data frame to be written to the table specified in tableName.
#'
#' Please make sure that the column names and types of the data frame are consistent with the names and types in the EXASolution table.
#'
#' @param tableName Name of the table to write the data to. The table has to exist and the records will be appended.
#' @param tableColumns If your data frame contains only a subset of the columns you can specify these here. The columns and types have to be specified as a vector of strings like: \code{c("col1 INT", "col2 VARCHAR(20)")}
#'
#' Please look at the documentation of the cols parameter in \code{IMPORT INTO}
#' in the EXASolution manual for details.
#'
#' @param writer This parameter is for the rare cases where you want to customize the writer receiving the data frame and writing the data to the communication channel.
#' @param server This parameter is only relevant in rare cases where you want to customize the address of the data channel. Per default, the data channel uses the same host and port as the RODBC connection.
#'
#' @return The function returns the value returned by the writer, which is by
#'   default \code{NULL}.
#'
#' @author EXASOL AG <support@@exasol.com>
#'
#' @examples
#' require(RODBC)
#' require(exasol)
#'
#' # Connect via RODBC with configured DSN
#' C <- odbcConnect("exasolution")
#'
#' # Generate example data frame with two groups
#' # of random values with different means.
#' valsMean0  <- rnorm(10, 0)
#' valsMean50 <- rnorm(10, 50)
#' twogroups <- data.frame(group = rep(1:2, each = 10),
#'                         value = c(valsMean0, valsMean50))
#'
#' # Write example data to a table
#' odbcQuery(C, "CREATE SCHEMA test")
#' odbcQuery(C, "CREATE TABLE test.twogroups (groupid INT, val DOUBLE)")
#' exa.writeData(C, twogroups, tableName = "test.twogroups")
#'
#' @export
exa.writeData <- function(channel, data, tableName, tableColumns = NA,
                          writer = function(data, conn) write.table(data,
                                                                    file = conn,
                                                                    row.names = FALSE,
                                                                    col.names = FALSE,
                                                                    na = "",
                                                                    sep = ",",
                                                                    qmethod = "double"),
                          server = NA) {
  slot <- 0; m <- match.call(); m$tableColumns[[1]] <- NULL
  try(.Call(C_asyncRODBCQueryFinish, slot, 1))
  serverAddress <- strsplit(
    if(is.na(server)) odbcGetInfo(channel)[['Server_Name']]
    else server, ':')[[1]]
  serverHost <- as.character(serverAddress[[1]]); serverPort <- as.integer(serverAddress[[2]])
  .Call(C_asyncRODBCIOStart, slot, serverHost, serverPort)
  proxyHost <- .Call(C_asyncRODBCProxyHost, slot)
  proxyPort <- .Call(C_asyncRODBCProxyPort, slot)
  query <- paste("IMPORT INTO ", tableName,
                 if (is.null(m$tableColumns)) ""
                 else paste('(', do.call(paste, c(lapply(tableColumns, as.character),
                                                  sep = ", ")),
                            ')', sep = ''),
                 " FROM CSV AT 'http://", proxyHost, ":",
                 proxyPort, "' FILE 'importData.csv'", sep = "")
  on.exit(.Call(C_asyncRODBCQueryFinish, slot, 1))
  fd <- .Call(C_asyncRODBCQueryStart, slot, attr(channel, "handle_ptr"), query, 1)
  res <- writer(data, fd)
  flush(fd)
  on.exit(NULL)
  .Call(C_asyncRODBCQueryFinish, slot, 0)
  res
}
