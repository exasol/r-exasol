#' @include EXADBI-object.R
NULL

## Declaration of EXAConnection and implementation of connection related to DBI API.

#' The S3 class RODBC will be registered as a superclass of EXAConnection
#' @name RODBC-class
setOldClass("RODBC")


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
#' @slot db_user A string containing the database user name.
#' @slot db_name A string containing the database name.
#' @slot db_prod_name A string containing the database product name.
#' @slot db_version A string containing the database version.
#' @slot drv_name A string containing the connection driver version.
#' @slot encrypted A logical indicating if connection should be encrypted.
#' @author EXASOL AG <opensource@exasol.com>
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


# Connection -------------------------------------------------------------------

#' @title dbConnect
#' @describeIn dbConnect Creates a new connection to an EXASOL Database.
#'
#' @family EXADriver related objects
#' @family EXAConnection related objects
#'
#' @param drv An EXAdriver object, a character string "exasol", "exa" or "exasol_driver", or an
#'   existing EXAConnection object (for connection cloning).
#' @param exahost DNS or IP (or range of IPs) and port of the database cluster,
#'   e.g. '10.0.2.15..20:8563'
#' @param uid DB username, e.g. 'sys'
#' @param pwd DB user password, e.g. 'exasol'
#' @param schema Schema in EXASOL db which is opened directly after the
#'   connection.
#' @param exalogfile The EXASOL ODBC driver log file. By standard a tempfile is
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
#' @param autocommit By default 'Y'. If 'Y' each SQL statement is committed. 'N'
#'   means that no commits are executed automatically. The transaction will be
#'   rolled back on disconnect, which causes the loss of all data written during
#'   the transaction.
#' @param querytimeout Time EXASOL DB computes a query before it is aborted.
#'   The default \code{'0'} (zero) means no timeout, i.e. runs until finished.
#' @param connectionlcctype Sets the connection locale \code{LC CTYPE}.
#'   The default is the setting of the client's current R session.
#' @param connectionlcnumeric Sets the connection locale \code{LC NUMERIC}.
#'   The default is the setting of the client's current R session.
#' @param ... Additional parameters to the connection string. If a connection is
#'   cloned, these override the old connection settings.
#' @param dsn A preconfigured ODBC Data Source Name. Parameter being evaluated
#'   with priority to \code{EXAHOST}.
#' @param connection_string Alternatively to everything else, a custom ODBC
#'   connection sting can be provided. See EXASOL DB manual secion 4.2.5 for
#'   details, available at \url{https://docs.exasol.com/}.
#' @return A fresh EXAConnection object.
#' @examples \dontrun{
#'  con <- dbConnect("exa", dsn = "EXASolo")
#'  con <- dbConnect("exa", exahost = "212.209.123.20..25:8563",
#'                   uid = "peter", pwd = "password123", schema = "sales")
#' }
#' @include EXADBI-driver.R
#' @seealso \code{\link[DBI:dbConnect]{DBI::dbConnect()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
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
    .EXANewConnection(
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

#' @describeIn dbConnect Creates a connection to an EXASOL Database based on string.
#' @family EXADriver related objects
#' @family EXAConnection related objects
#'
#' @param drv Driver as character string which is being forwarded to \code{\link{dbDriver}}.
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbConnect", "character",
  definition = function(drv, ...)
    .EXANewConnection(drv = dbDriver(drv), ...),
  valueClass = "EXAConnection"
)

#' @describeIn dbConnect Creates a connection to an EXASOL Database based on an existing connection.
#' @family EXADriver related objects
#' @family EXAConnection related objects
#
#' @param drv Existing connection
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbConnect", "EXAConnection",
  definition = function(drv, ...)
    .EXACloneConnection(drv, ...),
  valueClass = "EXAConnection"
)


#' Fetches and outputs the current schema from an EXASOL DB. Also updates EXAConnection metadata.
#' @family EXAConnection related objects
#' @family None-DBI-API
#'
#' @name dbCurrentSchema
#' @param con A valid EXAConnection
#' @param setSchema If not NULL, this schema will be opened and attached to EXAConnection metadata.
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

.EXANewConnection <- function(# change defaults also above
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

  res <- new(
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
    db_name = exa_metadata["Data_Source_Name"],
    db_prod_name = exa_metadata["DBMS_Name"],
    db_version = exa_metadata["DBMS_Ver"],
    drv_name = exa_metadata["Driver_Name"],
    encrypted = ifelse(encryption == "Y",TRUE,FALSE),
    con
  )
  tryCatch({
    .on_connection_opened(res)
  }, error = function(e) {
    warning(paste0("Error opening connection pane:\n'", conditionMessage(e), "'"))
  })
  res
}

## Opens a new connection with the same settings as an existing one.
## @family EXADriver related objects
## @family EXAConnection related objects
##
## @param drv An EXAConnection object to be dublicated.
## @param autocommit A logical that if it is true, autocommit will be enabled for cloned connection.
## @param ... An additional connection string parameter that may override the old settings.
## @return A fresh EXAConnection
.EXACloneConnection <-
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
          ".EXACloneConnection error: failed to initialise connection.\nConnection String:", con_str
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
      db_name = exa_metadata["Data_Source_Name"],
      db_prod_name = exa_metadata["DBMS_Name"],
      db_version = exa_metadata["DBMS_Ver"],
      drv_name = exa_metadata["Driver_Name"],
      encrypted = drv@encrypted,
      con
    )
  }

#' @title dbDisconnect
#'
#' @describeIn dbDisconnect Disconnects the connection.
#'
#' @param conn An EXAConnection object.
#' @return A logical indicating success.
#' @export
#' @author EXASOL AG <opensource@exasol.com>
#' @seealso \code{\link[DBI:dbDisconnect]{DBI::dbDisconnect()}}
setMethod(
  "dbDisconnect",signature("EXAConnection"),
  definition = function(conn) {
    tryCatch({
      .on_connection_closed(conn)
    }, error = function(e) {
      warning(paste0("Error closing connection pane:\n'", conditionMessage(e), "'"))
    })
    odbcClose(conn)
  }
)

