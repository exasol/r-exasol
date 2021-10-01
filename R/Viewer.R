.odbcListObjectTypes <- function(connection) {
  # slurp all the objects in the database so we can determine the correct
  # object hierarchy

  # all databases contain tables, at a minimum
  obj_types <- list(table = list(contains = "data"))

  # # see if we have views too
  # table_types <- string_values(connection_sql_tables(connection@ptr, "", "", "", "%")[["table_type"]])
  # if (any(table_types == "VIEW")) {
  #   obj_types <- c(obj_types, list(view = list(contains = "data")))
  # }
  #
  # # check for multiple schema or a named schema
  # schemas <- string_values(connection_sql_tables(connection@ptr, "", "%", "", "")[["table_schema"]])
  # if (length(schemas) > 0) {
  #   obj_types <- list(schema = list(contains = obj_types))
  # }
  #
  # # check for multiple catalogs
  # catalogs <- string_values(connection_sql_tables(connection@ptr, "%", "", "", "")[["table_catalog"]])
  # if (length(catalogs) > 0) {
  #   obj_types <- list(catalog = list(contains = obj_types))
  # }

  obj_types
}

.odbcListObjects <- function(connection, catalog = NULL, schema = NULL, name = NULL, type = NULL, ...) {

  # # if no catalog was supplied but this database has catalogs, return a list of
  # # catalogs
  # if (is.null(catalog)) {
  #   catalogs <- string_values(connection_sql_tables(connection@ptr, catalog_name = "%", "", "", NULL)[["table_catalog"]])
  #   if (length(catalogs) > 0) {
  #     return(
  #       data.frame(
  #         name = catalogs,
  #         type = rep("catalog", times = length(catalogs)),
  #         stringsAsFactors = FALSE
  #     ))
  #   }
  # }
  #
  # # if no schema was supplied but this database has schema, return a list of
  # # schema
  # if (is.null(schema)) {
  #   schemas <- string_values(connection_sql_tables(connection@ptr, "", "%", "", NULL)[["table_schema"]])
  #   if (length(schemas) > 0) {
  #     return(
  #       data.frame(
  #         name = schemas,
  #         type = rep("schema", times = length(schemas)),
  #         stringsAsFactors = FALSE
  #     ))
  #   }
  # }
  #
  # objs <- tryCatch(connection_sql_tables(connection@ptr, catalog, schema, name, table_type = type), error = function(e) NULL)
  # # just return a list of the objects and their types, possibly filtered by the
  # # options above
  # data.frame(
  #   name = objs[["table_name"]],
  #   type = tolower(objs[["table_type"]]),
  #   stringsAsFactors = FALSE
  # )
  data.frame(
     name = c("Table A", "View B"),
     type = c("Talbe", "View"),
     stringsAsFactors = FALSE)
}

# given a connection, returns its "host name" (a unique string which identifies it)
.computeHostName <- function(connection) {
  connection@db_host
}

.computeDisplayName <- function(connection) {
  connection@db_name
}

# selects the table or view from arguments
.validateObjectName <- function(table, view) {

  # Error if both table and view are passed
  if (!is.null(table) && !is.null(view)) {
    stop("`table` and `view` can not both be used", call. = FALSE)
  }

  # Error if neither table and view are passed
  if (is.null(table) && is.null(view)) {
    stop("`table` and `view` can not both be `NULL`", call. = FALSE)
  }

  table %||% view
}

.odbcListColumns <- function(connection, table = NULL, view = NULL,
                                           catalog = NULL, schema = NULL, ...) {

  # specify schema or catalog if given
  # cols <- connection_sql_columns(connection@ptr,
  #   table_name = .validateObjectName(table, view),
  #   catalog_name = catalog,
  #   schema_name = schema)

  # extract and name fields for observer
  # data.frame(
  #   name = cols[["name"]],
  #   type = cols[["field.type"]],
  #   stringsAsFactors = FALSE)

  data.frame(
    name = c("Title A", "Title B"),
    type = c("Integer", "Varchar(100)"),
    stringsAsFactors = FALSE)

}

.odbcPreviewObject <- function(connection, rowLimit, table = NULL, view = NULL,
                                             schema = NULL, catalog = NULL, ...) {
  # extract object name from arguments
  name <- .validateObjectName(table, view)

  # prepend schema if specified
  if (!is.null(schema)) {
    name <- paste(dbQuoteIdentifier(connection, schema),
                  dbQuoteIdentifier(connection, name), sep = ".")
  }

  # prepend catalog if specified
  if (!is.null(catalog)) {
    name <- paste(dbQuoteIdentifier(connection, catalog), name, sep = ".")
  }

  dbGetQuery(connection, paste("SELECT * FROM", name), n = rowLimit)
}

.exasol_icon <- function() {
  paste0(system.file(package = packageName()), '/res/exasol.png')
}

.on_connection_closed <- function(con) {
  # make sure we have an observer
  observer <- getOption("connectionObserver")
  if (is.null(observer))
    return(invisible(NULL))

  type <- .dbms_type()
  host <- .computeHostName(con)
  observer$connectionClosed(type, host)
}

.dbms_type <- function() {
  "Exasol Db"
}

.on_connection_updated <- function(con, hint) {
  # make sure we have an observer
  observer <- getOption("connectionObserver")
  if (is.null(observer))
    return(invisible(NULL))

  type <- .dbms_type()
  host <- .computeHostName(con)
  observer$connectionUpdated(type, host, hint = hint)
}

.build_code <- function(metadata) {
  code <-
    c( paste("library(", packageName(), ")"),
       paste("con <- dbConnect(\"exa\", exahost = \"<hostname>:8563\", uid = \"sys\", pwd = \"<password>\", encrypted = \"Y\")"))
  paste(code, collapse = "\n")
}

.on_connection_opened <- function(connection) {
  # make sure we have an observer
  observer <- getOption("connectionObserver")
  if (!is.null(observer)) {
    # find an icon for this DBMS
    icon <- .exasol_icon()

    # let observer know that connection has opened
    observer$connectionOpened(
      # connection type
      type = .dbms_type(),

      # name displayed in connection pane
      displayName = .computeDisplayName(connection),

      # host key
      host = .computeHostName(connection),

      # icon for connection
      icon = icon,

      # connection code
      connectCode = .build_code(),

      # disconnection code
      disconnect = function() {
        dbDisconnect(connection)
      },

      listObjectTypes = function () {
        .odbcListObjectTypes(connection)
      },

      # table enumeration code
      listObjects = function(...) {
        .odbcListObjects(connection, ...)
      },

      # column enumeration code
      listColumns = function(...) {
        .odbcListColumns(connection, ...)
      },

      # table preview code
      previewObject = function(rowLimit, ...) {
        .odbcPreviewObject(connection, rowLimit, ...)
      },

      # raw connection object
      connectionObject = connection
    )
  }
}
# nocov end
