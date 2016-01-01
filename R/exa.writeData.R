#' Write a data.frame into an EXASOL table fast.
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
#' Please make sure that the column names and types of the data frame are consistent with the names
#' and types in the EXASolution table.
#'
#' @param tableName Name of the table to write the data to. The table has to exist and the records will be appended.
#' @param tableColumns If your data frame contains only a subset of the columns you can specify these
#' here. The columns and types have to be specified as a vector of strings like: \code{c("col1", "col2")}
#' Please take a look at the documentation of the cols parameter in the EXASolution User Manual sec. 2.2.2 '\code{IMPORT}', for details.
#'
#' @param writer This parameter is for the rare cases where you want to customize the writer receiving
#' the data frame and writing the data to the communication channel.
#' @param server This parameter is only relevant in rare cases where you want to customize the address
#' of the data channel. Per default, the data channel uses the same host and port as the RODBC connection.
#'
#' @return The function returns the value returned by the writer, or TRUE if there is none.
#'
#' @author EXASOL AG <support@@exasol.com>
#' @example examples/writeData.R
#' @export
exa.writeData <- function(channel, data, tableName, tableColumns=NA,
                          writer = function(data, conn) {
                            write.table(data,
                                        file = conn,
                                        row.names = FALSE,
                                        col.names = FALSE,
                                        na = "",
                                        sep = ",",
                                        qmethod = "double")
                          },
                          server) {
  slot <- 0

  try(.Call(C_asyncRODBCQueryFinish, slot, 1))

  if (missing(server)) {
    server <- odbcGetInfo(channel)[["Server_Name"]]
  }

  serverAddress <- strsplit(server, ":")[[1]]

  serverHost <- as.character(serverAddress[[1]])
  serverPort <- as.integer(serverAddress[[2]])

  .Call(C_asyncRODBCIOStart, slot, serverHost, serverPort)
  proxyHost <- .Call(C_asyncRODBCProxyHost, slot)
  proxyPort <- .Call(C_asyncRODBCProxyPort, slot)

  query <- paste0("IMPORT INTO ", tableName,
                 if (is.na(tableColumns)) ""
                 else {paste("(",paste(tableColumns,collapse=", "),")")},
                 " FROM CSV AT 'http://", proxyHost, ":",
                 proxyPort, "' FILE 'importData.csv'")
  on.exit(.Call(C_asyncRODBCQueryFinish, slot, 1))

  fd <- .Call(C_asyncRODBCQueryStart, slot,
              attr(channel, "handle_ptr"), query, 1)

  res <- writer(data, fd)
  flush(fd)
  .Call(C_asyncRODBCQueryFinish, slot, 0)
  ifelse(is.null(res), return(TRUE), return(res))
}
