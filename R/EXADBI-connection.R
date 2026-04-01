#' @include EXADBI-object.R
NULL

## Declaration of EXAConnection and implementation of connection related to DBI API.

#' An Object holding a connection to an EXASOL Database.
#'
#' @seealso \code{\link{DBIConnection-class}}
#' @family DBI classes
#' @family EXAConnection related objects
#'
#' @slot ws_handle An external pointer to the C++ WebSocket session.
#' @slot session_id An integer containing the Exasol session ID.
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
    ws_handle = "externalptr",
    session_id = "numeric",
    current_schema = "character",
    autocom_default = "logical",
    db_host = "character",
    db_port = "numeric",
    db_user = "character",
    db_pwd = "character",
    db_name = "character",
    db_prod_name = "character",
    db_version = "character",
    drv_name = "character",
    encrypted = "logical"
  ),
  contains = c("DBIConnection", "EXAObject")
)


# Connection -------------------------------------------------------------------

#' @title dbConnect
#' @describeIn dbConnect Creates a new connection to an EXASOL Database via WebSocket.
#'
#' @family EXADriver related objects
#' @family EXAConnection related objects
#'
#' @param drv An EXAdriver object, a character string "exasol", "exa" or "exasol_driver", or an
#'   existing EXAConnection object (for connection cloning).
#' @param exahost Host and port of the database cluster, e.g. '10.0.2.15:8563'
#'   or '10.0.2.15..20:8563'.
#' @param uid DB username, e.g. 'sys'
#' @param pwd DB user password, e.g. 'exasol'
#' @param schema Schema in EXASOL db which is opened directly after the
#'   connection.
#' @param encryption Whether to use TLS encryption. 'Y' (default) enables TLS.
#' @param autocommit By default 'Y'. If 'Y' each SQL statement is committed. 'N'
#'   means that no commits are executed automatically. The transaction will be
#'   rolled back on disconnect, which causes the loss of all data written during
#'   the transaction.
#' @param ... Additional parameters (currently unused).
#' @return A fresh EXAConnection object.
#' @examples \dontrun{
#'  con <- dbConnect("exa", exahost = "212.209.123.20:8563",
#'                   uid = "peter", pwd = "password123", schema = "sales")
#' }
#' @include EXADBI-driver.R
#' @seealso \code{\link[DBI:dbConnect]{DBI::dbConnect()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbConnect", "EXADriver",
  definition = function(drv,
                        exahost = "",
                        uid = "",
                        pwd = "",
                        schema = "SYS",
                        encryption = "Y",
                        autocommit = "Y",
                        ...)
  {
    .EXANewConnection(
      drv = drv,
      exahost = exahost,
      uid = uid,
      pwd = pwd,
      schema = schema,
      encryption = encryption,
      autocommit = autocommit,
      ... = ...
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
    .Call(C_exaWsExecute, con@ws_handle, paste("open schema", processIDs(setSchema)))
    con@current_schema <- setSchema
  } else {
    res <- .Call(C_exaWsExecute, con@ws_handle, "select current_schema")
    if (!is.null(res$data) && length(res$data) > 0 && length(res$data[[1]]) > 0) {
      con@current_schema <- as.character(res$data[[1]][1])
    }
  }
  message(paste("Schema: ", con@current_schema))
  con
}

.EXANewConnection <- function(drv,
                              exahost = "",
                              uid = "",
                              pwd = "",
                              schema = "SYS",
                              encryption = "Y",
                              autocommit = "Y",
                              ...) {
  if (exahost == "" || uid == "") {
    stop("Connect failed. Host (exahost) and username (uid) are required.")
  }

  parts <- strsplit(exahost, ":")[[1]]
  host <- parts[1]
  port <- as.integer(parts[2])
  if (is.na(port)) port <- 8563L

  useTls <- (encryption == "Y")

  result <- .Call(C_exaWsConnect, host, port, useTls, uid, pwd, 3L)

  if (!is.na(schema) && nchar(schema) > 0 && schema != "SYS") {
    .Call(C_exaWsExecute, result$handle, paste("OPEN SCHEMA", processIDs(schema)))
  }

  autocommit_json <- ifelse(autocommit == "Y",
                            '{"autocommit":true}',
                            '{"autocommit":false}')
  .Call(C_exaWsSetAttributes, result$handle, autocommit_json)

  res <- new("EXAConnection",
    ws_handle = result$handle,
    session_id = as.numeric(result$sessionId),
    current_schema = schema,
    autocom_default = (autocommit == "Y"),
    db_host = host,
    db_port = as.numeric(port),
    db_user = uid,
    db_pwd = pwd,
    db_name = result$dbName,
    db_prod_name = result$prodName,
    db_version = result$dbVersion,
    drv_name = "r-exasol",
    encrypted = useTls
  )

  tryCatch({
    .on_connection_opened(res)
  }, error = function(e) {
    warning(paste0("Error opening connection pane:\n'", conditionMessage(e), "'"))
  })
  res
}

.EXACloneConnection <-
  function(drv, autocommit, ...) {
    drv <- dbCurrentSchema(drv)

    autocom <- if (!missing(autocommit)) (autocommit == "Y") else drv@autocom_default

    .EXANewConnection(
      drv = new("EXADriver"),
      exahost = paste0(drv@db_host, ":", as.integer(drv@db_port)),
      uid = drv@db_user,
      pwd = drv@db_pwd,
      schema = drv@current_schema,
      encryption = ifelse(drv@encrypted, "Y", "N"),
      autocommit = ifelse(autocom, "Y", "N"),
      ...
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
    .Call(C_exaWsDisconnect, conn@ws_handle)
    invisible(TRUE)
  }
)

