require(RODBC)

SET <- quote(SET)
SCALAR <- quote(SCALAR)
EMITS <- quote(EMITS)
RETURNS <- quote(RETURNS)

exa.readData <- function(channel, query, reader = function(...) read.csv(..., stringsAsFactors = FALSE, blank.lines.skip=FALSE), server = NA) {
  slot <- 0; query <- as.character(query)
  try(.Call(C_asyncRODBCQueryFinish, slot, 1))
  serverAddress <- strsplit(if(is.na(server)) odbcGetInfo(channel)[['Server_Name']] else server, ':')[[1]]
  serverHost <- as.character(serverAddress[[1]]); serverPort <- as.integer(serverAddress[[2]])
  .Call(C_asyncRODBCIOStart, slot, serverHost, serverPort)
  proxyHost <- .Call(C_asyncRODBCProxyHost, slot)
  proxyPort <- .Call(C_asyncRODBCProxyPort, slot)
  query <- paste("EXPORT (", query, ") INTO CSV AT 'http://",  proxyHost, ":",
                 proxyPort, "' FILE 'executeSQL.csv' WITH COLUMN NAMES", sep = "")
  on.exit(.Call(C_asyncRODBCQueryFinish, slot, 1))
  fd <- .Call(C_asyncRODBCQueryStart, slot, attr(channel, "handle_ptr"), query, 0)
  res <- reader(fd)
  on.exit(NULL)
  .Call(C_asyncRODBCQueryFinish, slot, 0)
  res
}

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

exa.createScript <- function(channel, name, func = NA,
                         env = list(),
                         initCode = NA,
                         cleanCode = NA,
                         inType = SET,
                         inArgs = list(),
                         outType = EMITS,
                         outArgs = list(),
                         outputAddress = NA,
                         replaceIfExists = TRUE) {
  m <- match.call()
  code <- func
  initCode <- m$initCode; cleanCode <- m$cleanCode
  inType <- if(is.null(m$inType)) quote(SET) else m$inType
  outType <- if(is.null(m$outType)) quote(EMITS) else m$outType

  #inArgs <- lapply(2:(length(m$inArgs)), function(x)
  #                 paste(deparse(m$inArgs[[x]][[2]]), deparse(m$inArgs[[x]][[1]])))
  inArgs <- do.call(paste, c(as.list(inArgs), sep = ", "))
  if (outType == quote(EMITS)) {
    if (is.null(m$outArgs))
      stop("No output arguments given")
    #outArgs <- paste("(", do.call(paste, c(lapply(2:(length(m$outArgs)), function(x)
    #                                              paste(deparse(m$outArgs[[x]][[2]]), deparse(m$outArgs[[x]][[1]]))),
    #                                       sep = ", ")), ")")
    outArgs <- paste("(", do.call(paste, c(as.list(outArgs), sep = ", ")), ")", sep = "")
  } else {
    outType <- quote(RETURNS)
    outArgs <- as.character(outArgs)
  }
  #print(paste('###', inArgs))
  #print(paste('###', outArgs))

  sql <- paste("CREATE", if (replaceIfExists) "OR REPLACE" else "", "R",
                deparse(inType), "SCRIPT", name,
                "(", inArgs, ")",
                deparse(outType), outArgs, "AS")

  if (!is.null(m$outputAddress))
    sql <- paste(sql,
                 "# activate output to external server",
                 paste("output_connection__ <- socketConnection('", outputAddress[[1]], "', ", outputAddress[[2]], ")", sep = ''),
                 "sink(output_connection__)",
                 "sink(output_connection__, type = \"message\")",
                 "# ----------------------------------",
                 sep = "\n")
  if (!is.null(m$env))
    sql <- paste(sql, "\nenv <- ", do.call(paste, c(as.list(deparse(env)), sep = "\n")), "\n", sep = "")
  if (!is.null(initCode)) {
    sql <- paste(sql, "\n# code from the init function")
    for(codeLine in deparse(initCode))
      sql <- paste(sql, codeLine, sep = "\n")
    sql <- paste(sql, "\n# ---------------------------")
  }
  if (!is.null(cleanCode)) {
    sql <- paste(sql, "cleanup <- function()", sep = "\n")
    for(codeLine in deparse(cleanCode))
      sql <- paste(sql, codeLine, sep = "\n")
  }
  sql <- paste(sql, "run <-", sep = "\n")
  for(codeLine in deparse(code))
    sql <- paste(sql, codeLine, sep = "\n")
  sql <- paste(sql, "", sep = "\n")
  if (odbcQuery(channel, sql) == -1)
    stop(odbcGetErrMsg(channel)[[1]])

  function(..., table = NA, where = NA, groupBy = NA, restQuery = "", returnSQL = FALSE, reader = NA, server = NA) {
    m <- match.call(expand.dots = FALSE)
    args <- c(...)
    args <- lapply(1:(length(args)), function(x) as.character(args[[x]]))
    sql <- paste("SELECT * FROM (SELECT ",
                 name, "(", do.call(paste, c(args, sep = ", ")), ")",
                 " FROM ", table,
                 if(!is.null(m$where)) paste(" WHERE", as.character(where)),
                 if(!is.null(m$groupBy)) paste(" GROUP BY", as.character(groupBy)),
                 ")",
                 if(!is.null(m$restQuery)) paste("", as.character(restQuery)),
                 sep = "")
    if (returnSQL) paste('(', sql, ')', sep='')
    else {
      execArgs <- list(channel, sql)
      if (!is.null(m$reader)) execArgs <- c(execArgs, reader = reader)
      if (!is.null(m$server)) execArgs <- c(execArgs, server = server)
      do.call(exa.readData, execArgs)
    }
  }
}

# require(RODBC); require(exasol)
# cnx <- odbcDriverConnect("Driver=/var/Executables/bc/install/ok7500-e8/lib/libexaodbc-uo2214.so;UID=sys;PWD=exasol;EXAHOST=cmw72;EXAPORT=8563")
# sqlQuery(cnx, "OPEN SCHEMA TEST")
# require(RODBC); require(exasol); cnx <- odbcDriverConnect("Driver=/var/Executables/bc/install/ok7500-e8/lib/libexaodbc-uo2214.so;UID=sys;PWD=exasol;EXAHOST=cmw67;EXAPORT=8563"); sqlQuery(cnx, "OPEN SCHEMA TEST")

#cnx <- odbcDriverConnect("Driver=/var/Executables/bc/install/ok7500-e8/lib/libexaodbc-uo2214.so;UID=sys;PWD=exasol;EXAHOST=cmw72;EXAPORT=8563")
#testScript <- exa.createScript(cnx, testScript,
#env = list(a = 1, b1 = 2, b2 = 2, b3 = 2, b4 = 2, b5 = 2, b6 = 2, b7 = 2, b8 = 2, b9 = 2, ba = 2, bo = 2, be = 2, bu = 2, bi = 2, bd = 2, bh = 2, bt = 2, bn = 2),
#inArgs = { INT(a) },
#outArgs = { INT(b); INT(c) },
#outputAddress = c('192.168.5.61', 3000),
#initCode = {
#  require(RODBC); require(data.table)
#  print(paste("initialize", exa$meta$vm_id));
#},
#func = function(data) {
#  print("begin group")
#  data$next_row(NA);
#  data$emit(data$a, data$a + 3);
#  print("end group")
#})
#
#
#res <- testScript(1, test)
#res <- exa.readData(cnx, 'select testScript(1) from test')
#exa.writeData(cnx, test)
#
#res <- sqlQuery(cnx, 'select testScript(1) from test')

# print(testScript(int_index, table = enginetable, groupBy = mod(int_index, 4), returnSQL = TRUE))
# print(summary(testScript(int_index, table = enginetable, groupBy = mod(int_index, 4))))

# require(RODBC); require(exasol); cnx <- odbcDriverConnect("DSN=EXA"); sqlQuery(cnx, "open schema test"); exa.readData(cnx, "select * from cat")

