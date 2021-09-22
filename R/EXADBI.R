#' @include EXADBI-internal.R
#' @include exa.readData.R
#' @include exa.writeData.R
#' @include exa.createScript.R
#'
## These are the DBI related classes and methods, which work as an abstraction layer over the basic
## exa functions and provide compabibility to the DBI package. The structure may also serve as a
## foundation for a later implemntation of a proprietary CLI interface that does not depend on RODBC.
##
## First version written in 2015 by Marcel Boldt <marcel.boldt@exasol.com>
## as part of the EXASOL R interface & SDK package. It may be used, changed and distributed freely
## with no further restrictions than already stipulated in the package license, with the exception
## that this statement must stay included and unchanged.

#' @export dbDriver
#' @export dbUnloadDriver
#' @export dbConnect
#' @export dbListConnections

#' @export dbDisconnect
#' @export dbSendQuery
#' @export dbGetQuery
#' @export dbGetException
#' @export dbListResults
#' @export dbListFields
#' @export dbListTables
#' @export dbReadTable
#' @export dbWriteTable
#' @export dbExistsTable
#' @export dbRemoveTable
#' @export dbBegin
#' @export dbCommit
#' @export dbRollback

#' @export dbFetch
#' @export dbClearResult
#' @export dbColumnInfo
#' @export dbGetStatement
#' @export dbHasCompleted
#' @export dbGetRowsAffected
#' @export dbGetRowCount
NULL


# Class definitions ------------------------------------------------------------

#' EXAObject class.
#'
#' The virtual object constituting a basis to all other EXA DBI Objects.
#' @seealso \code{\link{DBIObject-class}}
#' @family DBI classes
#'
#' @docType class
setClass("EXAObject", contains = c("DBIObject", "VIRTUAL"))

#' Returns metadata on a given EXAObject.
#' @name dbGetInfo
#'
#' @param dbObj An EXAObject.
#' @return A named list.
#' @export
setMethod(
  "dbGetInfo", "EXAObject",
  definition = function(dbObj) {
    return("EXASOL DBI Object.")
  }
)

setMethod(
  "summary", "EXAObject",
  definition = function(object, ...) {
    NextMethod(generic = "summary", object, ...)
  }
)

# the S3 class RODBC will be registered as a superclass of EXAConnection
setOldClass("RODBC")

#' An interface driver object to the EXASOL Database.
#'
#' @seealso \code{\link{DBIDriver-class}}
#' @family DBI classes
#' @family EXADriver related objects
#' @slot driver A string containing the path to the EXASOL ODBC driver file.
setClass("EXADriver",
         contains = c("DBIDriver", "EXAObject"),
         slots = c(odbc_drv = "character")
         )

# Instantiates an EXADriver object.
# @family EXADriver related objects
# @param driver The path to an ODBC driver file. If missing, the driver contained in the exasol package
# is used if possible, otherwise the driver installed on the system is used.
# If "SYSTEM": the EXASOL ODBC driver installed on the system is used immediately.
# Alternatively a path to an ODBC driver library can be provided.
# @param silent If TRUE, no message is print.
# @return An EXADriver object.
exasol <- function(driver = NULL, silent = FALSE) {

  #TODO: determine driver file according to OS
  # path.package("exasol")


  if(missing(driver) | is.null(driver)) {
    if (!silent) print("Using the included driver...")
    driver <- file.path(
      switch(
        Sys.info()['sysname'],
        'Darwin' = paste0(system.file(package = packageName()),
                          '/odbc/lib/darwin/x86_64/libexaodbc-io418sys.dylib'
                          ),  # Mac OS
        'Linux' = paste0(system.file(package = packageName()),
                         '/odbc/lib/linux/x86_64/libexaodbc-uo2214lv1.so'
                         ),     # Linux
        "{EXASolution Driver}"              # default
      )
    )
  } else if (driver =="SYSTEM") {
    if(!silent) print("Using the system driver...")
    driver <- "{EXASolution Driver}"
  } else {
    if (!silent) print(paste("Using the driver at", driver))
  }

  if (!silent) print("EXASOL driver loaded")
  new("EXADriver", odbc_drv = driver)
}

exa <- exasol # define an alias

#TODO: set method dbDriver

setMethod(
  "summary", "EXADriver",
  definition = function(object, ...)
    NextMethod(generic = "summary", object,...)
)




setMethod(
  "dbGetInfo", "EXADriver",
  definition = function(dbObj) {
    list(
      driver.version = packageVersion("exasol"),
      max.connections = 999,
      DBI.version = packageVersion("DBI"),
      RODBC.version = packageVersion("RODBC"),
      client.version = R.Version()$version.string
    )
  }
)


#' An Object holding a connection to an EXASOL Database.
#'
#' @seealso \code{\link{DBIConnection-class}}
#' @family DBI classes
#' @family EXAConnection related objects
#'
#' @slot init_connection_string A string containing the ODBC connection sting used to
#'     initialise the connection.
#' @slot current_schema A string reflecting the current schema.
#' @slot autocom_default A logical indicating if autocommit is active.
#' @slot db_host A string containing the hostname or IP.
#' @slot db_port An integer containing the connection port.
#' @slot db_user A string containing the DB user name.
#' @slot db_name A string containing the database name.
#' @slot db_prod_name A string containing the database product name.
#' @slot db_version A string containing the database version.
#' @slot drv_name A string containing the connection driver version.
#' @slot encrypted A logical indicating if connection should be encrypted.
#' @slot connection.string RODBC
#' @slot handle_ptr RODBC
#' @slot case RODBC
#' @slot id RODBC
#' @slot believedNRows RODBC
#' @slot colQuote RODBC
#' @slot tabQuote RODBC
#' @slot encoding RODBC
#' @slot rows_at_time RODBC
#' @export
EXAConnection <- setClass(
  "EXAConnection",
  slots = c(
    init_connection_string = "character",
    current_schema = "character",
    autocom_default = "logical",
    db_host = "character",
    db_port = "numeric",
    db_user = "character",
    db_name = "character",
    db_prod_name = "character",
    db_version = "character",
    drv_name = "character",
    encrypted = "logical"
  ),
  contains = c("DBIConnection", "EXAObject", "RODBC")
)

# db.version, dbname, username, host, port

setMethod(
  "dbGetInfo","EXAConnection",
  definition = function(dbObj) {
    if (!dbIsValid(dbObj)) {
      stop("Connection expired.")
    }
    list(
      db.version = paste(dbObj@db_prod_name, dbObj@db_version),
      dbname = dbObj@db_name,
      username = dbObj@db_user,
      host = dbObj@db_host,
      port = dbObj@db_port
    )

  }
)

setMethod(
  "summary", "EXAConnection",
  definition = function(object, ...) {
    NextMethod(generic = "summary", object,...)
  }
)

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

setMethod(
  "dbGetInfo","EXAResult",
  definition = function(dbObj) {
    if(dbObj$temp_result_tbl == "CLEARED") stop("GetInfo: Result set cleared.")
    list(
      statement = dbObj$statement,
      row.count = dbObj$rows_fetched,
      rows.affected = dbObj$rows_affected,
      has.completed = dbObj$is_complete,
      is.select = dbObj$with_output
    )
  }
)


setMethod(
  "dbHasCompleted", signature("EXAResult"),
  definition = function(res) {
    if(res$temp_result_tbl == "CLEARED") stop("GetInfo: Result set cleared.");
    return(res$is_complete);
  }
)


setMethod(
  "summary", "EXAResult",
  definition = function(object, ...) {
    NextMethod(generic = "summary", object,...)
  }
)

setMethod("dbGetRowsAffected", signature("EXAResult"), function(res) {
  return(res$rows_affected)
})


#' Checks if an EXAObject is still valid.
#'
#' @name dbIsValid
#' @param conn An object that inherits EXAObject.
#' @return A logical indicating if the connection still works.
setMethod(
  "dbIsValid", signature("EXAObject"),
  definition = function(dbObj) {
    return(TRUE) # TODO
  }
)


#' Determine the EXASOL data type of an object.
#'
#'@seealso \code{\link{dbDataType,DBIObject-method}}
#' @export
#' @name dbDataType
setMethod("dbDataType", "EXAObject", function(dbObj, obj, ...) {
  if(any(class(obj) %in% c("factor", "data.frame")))
    EXADataType(obj)
  else
    EXADataType(unclass(obj))
})



setGeneric("EXADataType", function(x)
  standardGeneric("EXADataType"))
setMethod("EXADataType", "data.frame", function(x) {
  vapply(x, EXADataType, FUN.VALUE = character(1), USE.NAMES = FALSE)
})
setMethod("EXADataType", "integer",  function(x)
  "INT")
setMethod("EXADataType", "numeric",  function(x)
  "DECIMAL(36,15)")
setMethod("EXADataType", "logical",  function(x)
  "BOOLEAN")
setMethod("EXADataType", "Date",     function(x)
  "DATE")
setMethod("EXADataType", "POSIXct",  function(x)
  "TIMESTAMP")
varchar <- function(x) {
  max_varchar_length = max(nchar(as.character(x)), na.rm=TRUE)
  if (is.infinite(max_varchar_length)) {
    paste0("CLOB")
  } else if (max_varchar_length == 0) {
    paste0("VARCHAR(1)")
  } else {
    paste0("VARCHAR(", max_varchar_length, ")")
  }
}
setMethod("EXADataType", "character", varchar)
setMethod("EXADataType", "factor",    varchar)
setMethod("EXADataType", "list", function(x) {
  vapply(x, EXADataType, FUN.VALUE = character(1), USE.NAMES = FALSE)
})

setMethod("EXADataType", "raw",  varchar)

#setOldClass("AsIs")
#setMethod("EXADataType", "ANY",  definition=function(x) EXADataType(unclass(x)))

setMethod("EXADataType", "ANY", function(x) {

  warning(paste("Unrecognised datatype:", x, "Trying to convert to varchar."))
  varchar(x)
})


setMethod(
  "dbListConnections", "EXADriver",
  definition =  function(drv, ...)
    dbGetInfo(drv, "connectionIds")[[1]]
)



# Connection -------------------------------------------------------------------

#' Creates a connection to an EXASOL Database.
#'
#' @family EXADriver related objects
#' @family EXAConnection related objects
#'
#' @name dbConnect
#' @param drv An EXAdriver object, a character string "exasol" or "exa", or an
#'   existing EXAConnection object (for connection cloning).
#' @param exahost DNS or IP (or range of IPs) and port of the database cluster,
#'   e.g. '10.0.2.15..20:8563'
#' @param uid DB username, e.g. 'sys'
#' @param pwd DB user password, e.g. 'exasol'
#' @param schema Schema in EXASOL db which is opened directly after the
#'   connection.
#' @param exalogfile the EXASOL odbc driver log file. By standard a tempfile is
#'   created. Log data may be accessed with 'EXAlog(EXAConnection)'.
#' @param logmode EXASOL ODBC driver log mode. Allowed options are:
#' \describe{
#'  \item{NONE}{no log is written (default)}
#'  \item{DEFAULT}{most important function calls & SQL commands}
#'  \item{VERBOSE}{also additional data about internal steps & result data}
#'  \item{ON ERROR ONLY}{only errors are logged}
#'  \item{DEBUGCOMM}{extended logs, similar to verbose but w/o data & parameter
#'  tables}
#' }
#' @param encryption ODBC encryption. By default off. Switch on with 'Y'.
#' @param autocommit By default 'Y'. If Y' each SQL statement is committed. 'N'
#'   means that no commits are executed automatically. The transaction will be
#'   rolled back on disconnect, which causes the loss of all data written during
#'   the transaction.
#' @param querytimeout Time EXASOL DB computes a query before it is aborted.
#'   The default \code{'0'} (zero) means no timeout, i.e. runs until finished.
#' @param connectionlcctype Sets the connection locale \code{LC CTYPE}.
#'   The default is the setting of the client's current R session.
#' @param connectionlcnumeric Sets the connection locale \code{LC NUMERIC}.
#'   The default is the setting of the client's current R session.
#' @param encoding The connection encoding. TODO.
#' @param ... Additional parameters to the connection string. If a connection is
#'   cloned, these override the old connection settings.
#' @param dsn A preconfigured ODBC Data Source Name. Parameter being evaluated
#'   with priority to \code{EXAHOST}.
#' @param connection_string alternatively to everything else, a custom ODBC
#'   connection sting can be provided. See EXASOL DB manual secion 4.2.5 for
#'   details, available at \url{https://docs.exasol.com/}.
#' @return A fresh EXAConnection object.
#' @examples \dontrun{
#'  con <- dbConnect("exa", dsn = "EXASolo")
#'  con <- dbConnect("exa", exahost = "212.209.123.20..25:8563",
#'                   uid = "peter", pwd = "password123", schema = "sales")
#' }
setMethod(
  "dbConnect", "EXADriver",
  definition = function(drv, # change defaults also below
                        exahost = "",
                        uid = "",
                        pwd = "",
                        schema = "SYS",
                        exalogfile = tempfile(pattern = "EXAODBC_", fileext = ".log"),
                        logmode = "NONE",
                        encryption = "N",
                        autocommit = "Y",
                        querytimeout = "0",
                        connectionlcctype = Sys.getlocale(category = "LC_CTYPE"),
                        connectionlcnumeric = Sys.getlocale(category = "LC_NUMERIC"),
                        ...,
                        dsn = "",
                        connection_string = "")
  {
    EXANewConnection(
      drv = drv,
      exahost = exahost,
      uid = uid,
      pwd = pwd,
      schema = schema,
      exalogfile = exalogfile,
      logmode = logmode,
      encryption = encryption,
      autocommit = autocommit,
      querytimeout = querytimeout,
      connectionlcctype = connectionlcctype,
      connectionlcnumeric = connectionlcnumeric,
      ... = ...,
      dsn = dsn,
      connection_string = connection_string
    )
  },
  valueClass = "EXAConnection"
)


# @family EXADriver related objects
# @family EXAConnection related objects
#
# @inheritParams dbConnect
setMethod(
  "dbConnect", "character",
  definition = function(drv, ...)
    EXANewConnection(drv = dbDriver(drv), ...),
  valueClass = "EXAConnection"
)

# @family EXADriver related objects
# @family EXAConnection related objects
#
# @inheritParams dbConnect
setMethod(
  "dbConnect", "EXAConnection",
  definition = function(drv, ...)
    EXACloneConnection(drv, ...),
  valueClass = "EXAConnection"
)


#' Fetches and outputs the current schema from an EXASOL DB. Also updates EXAConnection metadata.
#' @family EXAConnection related objects
#'
#' @name dbCurrentSchema
#' @param con a valid EXAConnection
#' @return an updated EXAConnection
#' @export
dbCurrentSchema <- function(con, setSchema=NULL) {
  if(!missing(setSchema)) {
    sqlQuery(con, paste("open schema", processIDs(setSchema)))
    con@current_schema <- setSchema
  } else {
    res <- sqlQuery(con, "select current_schema")
    con@current_schema <- as.character(res[1,1])
  }
  message(paste("Schema: ", con@current_schema))
  con
}



EXANewConnection <- function(# change defaults also above
  drv,
  exahost = "",
  uid = "",
  pwd = "",
  schema = "SYS",
  exalogfile = tempfile(pattern = "EXAODBC_", fileext = ".log"),
  logmode = "NONE",
  encryption = "N",
  autocommit = "Y",
  querytimeout = "0",
  connectionlcctype = Sys.getlocale(category = "LC_CTYPE"),
  connectionlcnumeric = Sys.getlocale(category = "LC_NUMERIC"),
  ...,
  dsn = "",
  connection_string = "") {
  exaschema <- c(schema)

  if (connection_string != "") {
    con_str <- connection_string
  }
  else {
    if (dsn != "") {
      con_str <- paste0("DSN=",dsn)
    }
    else if (exahost != "" & uid != "") {
            con_str <- paste0("DRIVER=", drv@odbc_drv ,";", "EXAHOST=",exahost)
    }
    else {
      stop(
        "Connect failed. Either DSN, host & db_user or a connection string must be given.\n
        Hint: No lazy declaration of connection parameters - these have to be stated ' dsn=...'.\n
        See also the examples in the help ('?dbConnect')."
      )
    }
    # all additional parameters...
    if (uid != "") {
      con_str <- paste0(con_str,";UID=",uid,";PWD=",pwd)
    }
    # EXASCHEMA
    if (exaschema != "SYS") {
      con_str <- paste0(con_str,";EXASCHEMA=",exaschema)
    }
    # EXALOGFILE
    con_str <- paste0(con_str,";EXALOGFILE=",exalogfile)

    # LOGMODE
    con_str <- paste0(con_str,";LOGMODE=",logmode)

    # locale
    con_str <-
      paste0(
        con_str,";CONNECTIONLCCTYPE=",connectionlcctype,";CONNECTIONLCNUMERIC=",connectionlcnumeric
      )

    # autocommit

    con_str <- paste0(con_str,";autocommit=",autocommit)
    con_str <- paste0(con_str, ";ENCRYPTION=", ifelse(encryption == "Y", "yes", "no"))
    # dots
    d <- list(...)
    while (length(d) > 0) {
      con_str <- paste0(con_str,";", names(d[1]),"=",d[1])
      d[1] <- NULL
    }
    }

  con <- odbcDriverConnect(con_str)
  exa_metadata <- odbcGetInfo(con)

  new(
    "EXAConnection",init_connection_string = con_str,
    current_schema = exaschema,
    autocom_default = ifelse(autocommit == "Y",TRUE,FALSE),
    db_host = strsplit(exa_metadata["Server_Name"],":")[[1]][1],
    db_port = as.numeric(strsplit(exa_metadata["Server_Name"],":")[[1]][2]),
    db_user = substring(
      regmatches(
        attributes(con)$connection.string, gregexpr("UID=[\\w]+?;", attributes(con)$connection.string,perl =
                                                      TRUE)
      )[[1]],
      5,
      nchar(regmatches(
        attributes(con)$connection.string, gregexpr("UID=[\\w]+?;", attributes(con)$connection.string,perl =
                                                      TRUE)
      )[[1]]) - 1
    ),
    db_name = exa_metadata["Data_Source_name"],
    db_prod_name = exa_metadata["DBMS_Name"],
    db_version = exa_metadata["DBMS_Ver"],
    drv_name = exa_metadata["Driver_Name"],
    encrypted = ifelse(encryption == "Y",TRUE,FALSE),
    con
  )
  }

# Opens a new connection with the same settings as an existing one.
# @family EXADriver related objects
# @family EXAConnection related objects
#
# @param drv an EXAConnection object to be dublicated.
# @param ... additional connection string parameter that may override the old settings.
# @return a fresh EXAConnection
EXACloneConnection <-
  function(drv, autocommit, ...) {
    # todo: parameters

    drv <- dbCurrentSchema(drv) # update schema metadata

    # dots
    d <- data.frame(...,stringsAsFactors = FALSE)
    names(d) <- toupper(names(d))
    con_str <- drv@init_connection_string
    s <- strsplit(con_str, ";")
    s <- sapply(s, strsplit, "=")
    s <- lapply(s, function(x)
      toupper(x))

    con_str <- ""

    while (length(s) > 0) {
      # as long as there is at least one parameter in S
      if (is.null(d[[s[[1]][1]]])) {
        # if the first parameter of s (S is the conn_str) is not in d (the dots
        # parameters)
        con_str <-
          paste0(con_str, ";", s[[1]][1],"=", s[[1]][2]) # take the s parameter
        s[1] <- NULL # delete the first parameter from S
      } else {
        # else take the value out of d, delete the parameter from d, then
        # delete the first S parameter
        con_str <-
          paste0(con_str, ";", s[[1]][1], "=", d[[s[[1]][1]]])
        d[[s[[1]][1]]] <- NULL
        s[1] <- NULL
      }
    } # add the remaining dots parameters

    while (ncol(d) > 0) {
      con_str <- paste0(con_str, ";", names(d)[1], "=", d[[1]])
      d[1] <- NULL
    }

    con_str <-
      substr(con_str,2,nchar(con_str)) # remove the initial semicolon

    con <- odbcDriverConnect(con_str)
    if (con == -1) {
      stop(
        paste(
          "EXACloneConnection error: failed to initialise connection.\nConnection String:", con_str
        )
      )
    }
    exa_metadata <- odbcGetInfo(con)
    new(
      "EXAConnection",
      init_connection_string = con_str,
      current_schema = drv@current_schema,
      autocom_default = ifelse(
        !missing(autocommit),ifelse(autocommit == "Y",TRUE,FALSE),drv@autocom_default
      ),
      db_host = strsplit(exa_metadata["Server_Name"],":")[[1]][1],
      db_port = as.numeric(strsplit(exa_metadata["Server_Name"],":")[[1]][2]),
      db_user = substring(
        regmatches(
          attributes(con)$connection.string, gregexpr("UID=[\\w]+?;", attributes(con)$connection.string,perl =
                                                        TRUE)
        )[[1]],
        5,
        nchar(regmatches(
          attributes(con)$connection.string, gregexpr("UID=[\\w]+?;", attributes(con)$connection.string,perl =
                                                        TRUE)
        )[[1]]) - 1
      ),
      db_name = exa_metadata["Data_Source_name"],
      db_prod_name = exa_metadata["DBMS_Name"],
      db_version = exa_metadata["DBMS_Ver"],
      drv_name = exa_metadata["Driver_Name"],
      encrypted = drv@encrypted,
      con
    )
  }

#' Sends a commit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbCommit
#' @param conn An EXAConnection object
#' @return a logical indicating success.
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

#' Rolls the current DB transaction back.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbRollback
#' @param conn An EXAConnection object
#' @return a logical indicating success.
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


#' Starts a DB transaction. In EXASOL, it disables autocommit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbBegin
#' @param conn An EXAConnection object
#' @return a logical indicating success.
setMethod("dbBegin", signature("EXAConnection"),
          function(conn) {
            odbcSetAutoCommit(conn, autoCommit = FALSE)
            #message("Transaction started.")
            return(TRUE)
          })


#' Ends a DB transaction. In EXASOL, it commits and reinstates the connection's standard autocommit mode.
#' This is an EXASOL specific addition to the DBI interface and may not work with other RDBMS.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbEnd
#' @param conn An EXAConnection object
#' @param commit Logical. on TRUE the transaction is commmitted, otherwise rolled back.
#' @return a logical indicating success.
#' @export
setGeneric(
  "dbEnd",
  def = function(conn,...)
    standardGeneric("dbEnd"),
  valueClass = "logical"
)

setMethod("dbEnd", signature("EXAConnection"),
          function(conn,commit = TRUE, silent = FALSE) {
            ifelse(commit, dbCommit(conn, silent = silent), dbRollback(conn))
            odbcSetAutoCommit(conn, autoCommit = conn@autocom_default)
            # message("Transaction completed.")
            return(TRUE)
          })

#' Disconnects the connection.
#'
#' @name dbDisconnect
#' @param conn An EXAConnection object.
#' @return A logical indicating success.
setMethod(
  "dbDisconnect",signature("EXAConnection"),
  definition = function(conn) {
    odbcClose(conn)
  }
)



# Querying & Result ------------------------------------------------------------

#' Sends an SQL statment to an EXASOL DB, prepares for result fetching.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbSendQuery
#' @param conn A valid EXAConnection
#' @param statement vector mode character : an SQL statement to be executed in EXASOL db
#' @param schema vector mode character : a focus schema. This must have write access for the result
#'      set to be temporarily stored. If the user has only read permission on the schema to be read,
#'      another schema may be entered here, and table identifiers in stmt parameter must be
#'      fully qualified (schema.table).
#' @param profile logical, default TRUE : collect profiling information
#' @param default_fetch_req numeric, default 100 :
#' @param ... additional parameters to be passed on to dbConnect (used to clone the connection to
#'      one without autocommit)
#' @return EXAResult object which can be used for fetching rows. It also contains metadata.
setMethod(
  "dbSendQuery",
  signature(conn = "EXAConnection", statement = "character"),
  definition = function(conn,
                        statement,
                        schema = "",
                        profile = TRUE,
                        default_fetch_rec = 100,
                        ...)
    EXAExecStatement(
      con = conn,
      stmt = statement,
      schema = schema,
      profile = profile,
      default_fetch_rec = default_fetch_rec,
      ... = ...
    ),
  valueClass = "EXAResult"
)

isSelectStatement <-
  function(statement) {
    return(grepl("^\\s*(\\/\\*.*\\*\\/)?\\s*(WITH.*)?SELECT",toupper(statement),perl=TRUE))
}

EXAExecStatement <-
  function(con, stmt, schema = "", profile = TRUE, default_fetch_rec = 100, ...) {
    if (isSelectStatement(stmt)){
      stmt_cmd <- "SELECT"
    } else {
      stmt_cmd <- "NOSELECT"
    }
    qtime <- Sys.time()
    err <- vector(mode = "character")

    if (profile) {
      err <- append(err,sqlQuery(con, "alter session set profile='ON'"))
    }

    dbBegin(con)
    on.exit(dbEnd(con,commit = FALSE))

    if (stmt_cmd == "SELECT") {# ---------------if select ----------------------------------------
      temp_schema <- FALSE
      tbl_name <-
        paste0("TEMP_",floor(rnorm(1,1000,100) ^ 2),"_CREATED_BY_R")
      # con <- dbConnect(con, autocommit="N",...) # clone the connection with autocommit=off

      ids <- EXAGetIdentifier(stmt, statement = TRUE)

      if (schema == "") {
        # try to grep schema from stmt
        if (length(ids)>0) schema <- ids[[length(ids)]][1]
        if (schema != "" & schema != "\"\"") {
          message(paste("Using Schema from statement:", schema))
        } else {
            if (con@current_schema != "SYS") {
            message(paste("Using connection schema: ", con@current_schema))
            schema <- con@current_schema
            }
         }
      }
      if (schema == "" || schema == "\"\"") {
        # if nothing helps use temp_schema
        schema <- tbl_name
        temp_schema <- TRUE
        err <- append(err, paste("Using temporary schema:", schema))
        message(paste("Using temporary schema:", schema))
      }
      schema <- processIDs(schema)

      if (temp_schema)
        err <- append(err, sqlQuery(con, paste("create schema", schema)))
      sq1 <- paste0("create table ", schema, ".", tbl_name," as (", stmt, ")")
      #print(paste("-sql: ", sq1, " -END"))
      errr <-
        try(sqlQuery(con, sq1, errors = FALSE))
      # on success this won't return anything

      # dbCommit(con)

      if (errr == -1) {
        warning(odbcGetErrMsg(con))
        err <- append(err, odbcGetErrMsg(con))
      } else {
        dbEnd(con, commit = TRUE)
       # on.exit(dbEnd(con, commit = TRUE)) # commit after select in order to store indices that may have been created.
      }

    } else {
      # if NOT SELECT ------------------
      #

      if (schema != "") {
        schema <- processIDs(schema)
        err1 <-
          try(sqlQuery(con, paste("open schema", schema), errors = FALSE))
        if (err1 == -1) {
          # schema cannot be opened
          warning(paste("Schema cannot be opened:", schema,"\n",err1))
          err <- append(err, odbcGetErrMsg(con))
        }
      }

      err2 <- try(sqlQuery(con, stmt, errors = FALSE))

      if (err2 == -1) {
        err <- append(err, odbcGetErrMsg(con))
        stop(paste("Query failed.\n", odbcGetErrMsg(con)))
      } else {
        #on.exit(
          dbEnd(con,commit = TRUE)
        #)
      }
    }

    sqlQuery(con,"flush statistics")

    if (stmt_cmd == "SELECT") {
      rc <- try(sqlQuery(con, paste0("select count(*) from ", schema, ".", tbl_name))[1,1], silent = TRUE)
      rowcount <- ifelse(is.numeric(rc), rc, 0)
    } else rowcount <- 0

    p <- exa.readData(
      con, "select
      session_id,
      stmt_id,
      part_id,
      command_name,
      object_name,
      object_rows,
      duration,
      cpu,
      temp_db_ram_peak,
      hdd_read,
      net
      from exa_user_profile_last_day
      where session_id = current_session and stmt_id=current_statement-4
      order by part_id desc"
    ) # current_statement: -2 if autocommit=N, otherwise -4, -3 if dbCommit (all +1 due to rowcount)

    cols <- data.frame()

    if (stmt_cmd == "SELECT") {
      if (errr != -1) {
        message(rowcount," rows prepared in ",sum(p$DURATION)," seconds.")
      }

      cols <- exa.readData(
        con, paste0(
          "select
          column_ordinal_position,
          column_name, column_comment,
          column_type, column_maxsize,
          column_is_nullable,
          column_default,
          column_identity,
          column_owner,
          column_is_distribution_key
          from exa_user_columns
          where column_schema = ", processIDs(schema, quotes="'"),
          " and column_table = ", processIDs(tbl_name, quotes="'")
        )
        )

      res_tbl <- paste0(schema,".",tbl_name)
  } else {
      res_tbl <- ""
  }

    EXAResult$new(
      connection = con,
      statement = stmt,
      rows_fetched = 0,
      rows_affected = rowcount,
      is_complete = ifelse(stmt_cmd == "SELECT",FALSE,TRUE),
      with_output = ifelse(stmt_cmd == "SELECT",TRUE,FALSE),
      profile = p,
      columns = cols,
      temp_result_tbl = res_tbl,
      query_sent_time = qtime,
      errors = err,
      default_fetch_rec = default_fetch_rec
    )
  }

#' Fetches a subset of an result set.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @name dbFetch
#' @aliases fetch
#' @param res An EXAResult object.
#' @param n An int declaring the size of the subset to fetch. If missing, the whole subset is fetched.
#' @param ... further arguments to be passed on to exa.readData.
setMethod(
  "fetch", signature(res = "EXAResult", n = "numeric"),
  definition = function(res,n,...)
    EXAFetch(res,n,...)
)

# @family EXAResult related objects
# @family DQL functions
#
# @inheritParams fetch
# @export
setMethod(
  "fetch",signature(res = "EXAResult", n = "missing"),
  definition = function(res,...)
    EXAFetch(res,...)
)



EXAFetch <- function(res, n = res$default_fetch_rec, ...) {
  if(res$temp_result_tbl == "CLEARED") {
    stop("Fetch: trying to fetch from a cleared EXAResult.")
  }
  if (res$temp_result_tbl == "" | is.na(res$temp_result_tbl)) {
    warning("Fetch: Invalid EXAResult or no result set composed.")
    return(data.frame())
  }
  if (res$with_output & !res$is_complete) {
    if (n == -1) {
      n <- res$rows_affected
    }
    query <-
      paste(
        "select * from",res$temp_result_tbl,"order by rownum limit",n,"offset",res$rows_fetched
      )
    df <- exa.readData(res$connection, query,...)
    res$rows_fetched <- res$addRowsFetched(nrow(df))
    if (res$rows_fetched >= res$rows_affected) {
      res$is_complete <- TRUE
    }
    #print("\n--------------\n")
    #print(df)
    #print("\n--------------\n")
    return(df)
  } else {
    warning("Fetch: No more to fetch.")
    return(data.frame())
  }
}


#' Frees all resources associated with an \code{EXAResult}.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @name dbClearResult
#' @param res An EXAResult object.
#' @param ... Further arguments to passed to res$close(). This may be 'commit=TRUE' (not advisable).
#' @return A logical indicating success.
#' @export
setMethod(
  "dbClearResult", signature(res = "EXAResult"),
  definition = function(res,...)
    EXAClearResult(res,...)
)

EXAClearResult <- function(res,...) {
  # close is in row 203

  if (res$temp_result_tbl == "CLEARED") {
    message("Clear result: already cleared.")
    return(TRUE)
  }
  if (!res$with_output |
      res$temp_result_tbl == "") {
    # if not a SELECT stmt OR nothing to drop...
    #res$close()
    message("No result set to clear.")
    res$temp_result_tbl <- "CLEARED"
    return(TRUE)
  } else {
    # if a SELECT stmt...
    # 1. drop the table...
    err <-
      try(sqlQuery(res$connection, paste("drop table",res$temp_result_tbl), errors =
                     FALSE))
    if (err == -1) {
      stop(paste(
        "Couldn't remove temporary table. Delete:", res$temp_result_tbl
      ))
      return(FALSE)
    }
    stbl <-
      strsplit(res$temp_result_tbl,".",fixed = TRUE) # 2. check if the schema had been created...
    if (stbl[[1]][1] == stbl[[1]][2] &
        gregexpr("CREATED_BY_R",stbl[[1]][1])[[1]][1] > 0) {
      # if the tbl_name & schemaname are equal and contain 'CREATED_BY_R'...
      err <-
        try(sqlQuery(con, paste("drop schema",stbl[[1]][1]), errors = FALSE))
      # ...drop schema if empty
      if (err == -1) {
        stop(paste("Couldn't remove temp. schema:",stbl[[1]][1],"\n",err))
        return(FALSE)
      }
    }
    res$temp_result_tbl <- "CLEARED"
    return(TRUE) # if table (and schema) has been removed return true
  }

}

#' Executes the query, fetches and returns the entire result set.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbGetQuery
#' @param conn An EXAConnection object.
#' @param statement An SQL query statement to be executed in an EXASOL DB.
#' @param ... further arguments to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
setMethod(
  "dbGetQuery", signature("EXAConnection","character"),
  definition = function(conn, statement,...) {
    if (isSelectStatement(statement)) {
      return(exa.readData(conn,statement,...))
    } else {
      sqlQuery(conn, statement, errors = TRUE)
    }
  }
)

#' Lists all fields of a table.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbListFields
#' @param conn An EXAConnection object
#' @param schema Filter on DB schema
#' @param ... further parameters passed on to `exa.readData()`
#' @return a character vector. If no tables present, a character vector of length 0.
#' @export
setMethod(
  "dbListFields", signature("EXAConnection"),
  definition = function(conn, name, schema, ...) {

    if (missing(schema)) {
      ids <- EXAGetIdentifier(name, statement = FALSE)
      # try to grep schema from stmt
      if (length(ids)>0) {
        schema <- ids[[length(ids)]][1]
        name <- ids[[length(ids)]][2]
      }
      if (schema != "" & schema != "\"\"") {
       # message(paste("Using Schema from statement:", schema))
      } else {
         # message(paste("Using connection schema: ", con@current_schema))
          schema <- con@current_schema
      }
    }
    schema <- processIDs(schema, quotes = "'")
    name <- processIDs(name, quotes = "'")

    qstr <- paste0("select column_name from exa_all_columns where column_schema = ", schema, " and
                   column_table = ", name, " order by column_ordinal_position")
    res <- exa.readData(conn, qstr, ...)
    return(res$COLUMN_NAME)
  })

setMethod("dbListFields", signature("EXAResult"),
          definition = function(conn, ...) {
            conn$columns$COLUMN_NAME
          })

#' Lists all tables in the DB.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbListTables
#' @param conn An EXAConnection object
#' @param schema Filter on DB schema
#' @param ... further parameters passed on to `exa.readData()`
#' @return a character vector. If no tables present, a character vector of length 0.
#' @export
setMethod(
  "dbListTables", signature("EXAConnection"),
  definition = function(conn, schema, ...) {

    qstr <-
      paste0("select table_schema, table_name from exa_all_tables ", ifelse(!missing(schema), paste("where table_schema =",
             processIDs(schema,"'")), ""), " order by 1,2" )
    res <- exa.readData(conn, qstr, ...)
    return(paste0(res$TABLE_SCHEMA, ".", res$TABLE_NAME))
  })

#' Reads a DB table.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbReadTable
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name in the form schema.table.
#' @param schema Alternatively and with preference to `name`, a schema can be specified separately.
#' @param order_col A string containing columns to have the result set ordered upon, e.g. "col1 desc, col2"
#' @param limit A row limit to the result set.
#'        CAUTION: a limit without order clause is non-deterministic in EXASOL,
#'        i.e. subsequent runs of the same statement may deliver differing result sets.
#' @param ... further arguements to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
setMethod(
  "dbReadTable",
  signature("EXAConnection", "character"),
  definition = function(conn,
                        name,
                        schema = "",
                        order_col = NA,
                        limit = NA,
                        ...) {
    if (schema == "") {
      ids <- EXAGetIdentifier(name)
      schema <- ids[[1]][1]
      name <- ids[[1]][2]
    } else {
      schema <- processIDs(schema)
      name <- processIDs(name)
    }

    statement <-
      ifelse(schema != "",
             paste0("select * from ", schema, ".", name),
             paste0("select * from ", name))
    if (!is.na(order_col)) {
      statement <-
        paste(statement, "order by (", processIDs(order_col), ")")
    }
    if (!is.na(limit)) {
      statement <- paste(statement, "limit", limit)
    }
    exa.readData(conn, statement, ...)
  }
)



#' Checks if a table exists in an EXASOL DB.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbExistsTable
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name in the form schema.table.
#' @param schema Alternatively to `name`, a schema can be specified separately.
#' @return A logical indicating if the table exists.
setMethod(
  "dbExistsTable", signature("EXAConnection", "character"),
  definition = function(conn, name, schema = "") {
    if (schema == "") {
      ids <- EXAGetIdentifier(name, quotes = "'")
      schema <- ifelse(ids[[1]][1] != "\'\'", ids[[1]][1], processIDs(conn@current_schema, quotes="'"))
      name <- ids[[1]][2]
    } else {
      schema <- processIDs(schema, quotes = "'")
      name <- processIDs(name, quotes = "'")
    }

    qstr <-
      paste0("select * from exa_all_tables where table_schema = ",
             schema, " and table_name=", name)
    res <- sqlQuery(conn, qstr)
    if (nrow(res) == 0) {
      return(FALSE)
    } else if (nrow(res) == 1) {
      return(TRUE)
    } else if (nrow(res) > 1) {
      warning("Identifier ambiguous. Multiple matches.")
    } else {
      stop("Unknown error.")
    }
  }
)

#' Writes a data.frame into a table. If the table does not exist, it is created.
#' @family EXAConnection related objects
#' @family DML functions
#'
#' @name dbWriteTable
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name (schema.table). Alternatively the schema can be given
#'    via the parameter 'schema' (see below).
#' @param value A data.frame containing data.
#' @param schema A schema identifier.
#' @param field_types A character vector containing the column data types, in the form
#'     of c("varchar(20)","int").
#'        If missing, the column types of the data.frame are being converted and used.
#' @param overwrite A logical indicating if existing data shall be overwritten. Default is 'FALSE',
#'        i.e. new data is appended to the DB table.
#' @param writeCols a logical or a character vector containing the cols of the DB table to be
#'      written into, in the form of c("col1","col4","col3"). If set to TRUE, then the column names
#'      of the data.frame are used.
#'      If FALSE, NA, or missing, no write columns are defined and columns are matched by column order.
#'      Default is NA. Useful to change if the DB table contains more columns than the data.frame, or
#'      if the column order differs.
#' @param ... additional parameters to be passed on to exa.writeData.
#' @return a logical indicating success.
setMethod(
  "dbWriteTable", signature("EXAConnection", "character", "data.frame"),
  definition = function(conn, name, value, ...) {
    EXAWriteTable(conn, name, value,...)
  }
)

EXAWriteTable <-
  function(con, tbl_name, data, schema = "", field_types, overwrite = FALSE, writeCols =
             NA, ...) {
    if (schema == "") {
      ids <- EXAGetIdentifier(tbl_name)
      schema <- ids[[1]][1]
      tbl_name <- ids[[1]][2]
    } else {
      schema <- processIDs(schema)
      tbl_name <- processIDs(tbl_name)
    }

    dbBegin(con)
    on.exit(dbEnd(con,FALSE))

    if (dbExistsTable(con,paste0(schema,".",tbl_name))) {
      # if the table exists
      if (overwrite) {
        switch(as.character(sqlQuery(
          con, paste0("truncate table ",schema,".",tbl_name),errors = FALSE
        )),
        "-1" = stop(
          paste(
            "Error. Couldn't truncate table:",tbl_name,"\n",odbcGetErrMsg(con)
          )
        ),
        "-2" = message(paste(
          "Table",tbl_name,"successfully truncated."
        )),
        {
          print("Truncate failed.")
          stop(odbcGetErrMsg(con))
        })
      }
    } else {
      # tbl does not exist, create...
      ## DDL - table definition

      # field types
      if (missing(field_types)) {
        field_types <- dbDataType(con, data)
      } else {
        if (length(field_types) != ncol(data))
          stop(
            "Error creating database table: number of field
            types provided does not match number of data columns in data.frame."
          )
      }

      # column names
      col_names <- names(data)
      if (is.null(col_names)) {
        ## todo
        for (i in 1:ncol(data)) {
          col_names <- append(col_names, paste0("col_",i))
        }
      }

      # create the table definition
      # first check if the schema exists, otherwise create
      tryCatch({
        switch(as.character(sqlQuery(
          con, paste("open schema",schema),errors = FALSE
        )),
        "-1" = warning(
          paste("Cannot open schema",schema,". Trying to create...")
        ),
        "-2" = message(paste("Schema",schema, "found.")),
        {
          warning(odbcGetErrMsg(con))
        })
      },
      warning = function(war) {
        switch(as.character(sqlQuery(
          con, paste("create schema",schema),errors = FALSE
        )),
        "-1" = stop(paste(
          "failed. Couldn't create schema:",schema
        )),
        "-2" = message(paste(
          "Schema",schema,"successfully created."
        )),
        {
          print("failed.")
          stop(odbcGetErrMsg(con))
        })
      })
      # setting up the table definition string
      ddl_str <- paste0("create table ",schema,".",tbl_name, "( ")
      for (i in 1:length(col_names)) {
        ddl_str <-
          paste0(ddl_str, processIDs(col_names[i])," ", field_types[i], ", ")
      }
      ddl_str <-
        substr(ddl_str,1,nchar(ddl_str) - 2) # remove the final comma & space
      ddl_str <- paste0(ddl_str, " )")

      switch(as.character(sqlQuery(con,ddl_str,errors = FALSE)),
             "-1" = {
               stop(paste0(
                 "Couldn't create table: ",schema,".",tbl_name,":\n",odbcGetErrMsg(con)
               ))
             },
             "-2" = {
               message(paste0("Table ",schema,".",tbl_name," created:\n",ddl_str))
             },
             {
               print("failed.")
               stop(odbcGetErrMsg(con))
             })
      } # end of else (table creation)

    if (missing(writeCols) |
        writeCols[1] == FALSE) {
      writeCols <-
        NA
    } # if write cols are missing or NA, write w/o specifying col names.
    else if (writeCols[1] == TRUE) {
      writeCols <-
        names(data)
    } # if TRUE, use the data.frame colnames, else use whatever is in it

    message("Writing into table...")
    if (exa.writeData(con, data, paste0(schema,".",tbl_name),
                      tableColumns = processIDs(writeCols),...)) {
      on.exit(dbEnd(con))
      return(TRUE)
    }
    return(FALSE)
  }

#' Removes a table.
#' @family EXAConnection related objects
#' @family DML functions
#'
#' @name dbRemoveTable
#' @param conn An EXAConnection object.
#' @param name a fully qualified table identifier (schema.table) or a simple table identifier, if the
#' parameter schema is specified.
#' @param schema An optional string specifying the DB schema. Caution: use either this parameter, OR give
#'      a fully qualified identifier as giving both will result in an error.
#' @param cascade A logical indicating whether also foreign key constraints referencing the table to
#'      be deleted shall be removed. Default is FALSE.
#' @return A logicl indicating success.
setMethod(
  "dbRemoveTable", signature("EXAConnection"),
  definition = function(conn, name, schema = "", cascade = FALSE) {
    EXARemoveTable(conn, name, schema, cascade)
  }
)



EXARemoveTable <- function(con, tbl_name, schema, cascade = FALSE) {
  if (schema == "") {
    ids <- EXAGetIdentifier(tbl_name)
    schema <- ids[[1]][1]
    tbl_name <- ids[[1]][2]
  } else {
    schema <- processIDs(schema)
    tbl_name <- processIDs(tbl_name)
  }

  dbBegin(con)
  on.exit(dbEnd(con,FALSE))

  ddl_str <- paste0("DROP TABLE ",schema,".",tbl_name)
  if (cascade)
    ddl_str <- paste(ddl_str,"CASCADE CONSTRAINTS")
  switch(as.character(sqlQuery(con,ddl_str,errors = FALSE)),
         # "-1" = {stop(paste0("Couldn't remove table: ",schema,".",tbl_name,":\n",odbcGetErrMsg(con)))},
         "-2" = {
           message(paste0("Table ",schema,".",tbl_name," removed:\n",ddl_str))
           on.exit(dbEnd(con))
           return(TRUE)
         },
         {
           stop(paste0(
             "Couldn't remove table: ",schema,".",tbl_name,":\n",odbcGetErrMsg(con)
           ))
           return(FALSE)
         })
}

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