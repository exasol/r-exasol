.exa_tables <- function(connection, schema = NULL, types = c("VIEW", "TABLE")) {
  tables <- RODBC::sqlTables(connection)
  tables <- tables[tables$TABLE_TYPE %in% types,]
  if (!is.null(schema)) {
    tables <- tables[tables$TABLE_SCHEM %in% c(schema),]
  }
  tables
}

.odbcListObjectTypes <- function(connection) {
  # slurp all the objects in the database so we can determine the correct
  # object hierarchy

  # all databases contain tables, at a minimum
  obj_types <- list(table = list(contains = "data"))

  tables <- .exa_tables(connection)

  # see if we have views too
  table_types <- tolower(unique(tables$TABLE_TYPE))
  if ("view" %in% table_types) {
    obj_types <- c(obj_types, list(view = list(contains = "data")))
  }

  # assume we always have schemas!
  obj_types <- list(schema = list(contains = obj_types))

  obj_types
}

.list_table_names <- function(connection, schema, name = NULL) {
  tables <- .exa_tables(connection, schema, c("TABLE"))
  if (!is.null(name)) {
    tables <- tables[tables$TABLE_NAME %in% c(name),]
  }

  data.frame(
    name = tables$TABLE_NAME,
    type = rep("table", length(tables$TABLE_NAME)),
    stringsAsFactors = FALSE
  )
}

.list_view_names <- function(connection, schema, name = NULL) {
  views <- .exa_tables(connection, schema, c("VIEW"))
  if (!is.null(name)) {
    views <- views[views$TABLE_NAME %in% c(name),]
  }

  data.frame(
    name = views$TABLE_NAME,
    type = rep("view", length(views$TABLE_NAME)),
    stringsAsFactors = FALSE
  )
}

.odbcListObjects <- function(connection, catalog = NULL, schema = NULL, name = NULL, type = NULL, ...) {

  res <- data.frame()
  # if no schema was supplied, return a list of schema
  if (is.null(schema)) {
    tables <- .exa_tables(connection)
    schemas <- unique(tables$TABLE_SCHEM)
    if (length(schemas) > 0) {
      res <-
        data.frame(
          name = schemas,
          type = rep("schema", times = length(schemas)),
          stringsAsFactors = FALSE
      )
    }
  }
  else if (is.null(type)){
    tables_df <- .list_table_names(connection, schema, name)
    views_df <- .list_view_names(connection, schema, name)
    res <- rbind(tables_df, views_df)
  }
  else if (tolower(type) == "table") {
    res <- .list_table_names(connection, schema, name)
  }
  else if (tolower(type) == "view") {
    res <- .list_view_names(connection, schema, name)
  }
  res
}

# given a connection, returns its "host name" (a unique string which identifies it)
.computeHostName <- function(connection) {
  connection@db_host
}

.computeDisplayName <- function(connection) {
  connection_name <- connection@db_name
  if (is.na(connection_name) | connection_name == "") {
    connection_name <- paste0(connection@db_host, ":", connection@db_port)
  }
  connection_name
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

  ifelse(!is.null(table), table, view)
}

.odbcListColumns <- function(connection, table = NULL, view = NULL,
                                           catalog = NULL, schema = NULL, ...) {

  if (is.null(schema)) {
    stop("Schema must not be null")
  }
  name <- .validateObjectName(table, view)

  columns <- RODBC::sqlColumns(channel = connection, sqtable = name, schema = schema)

  res <- data.frame(
    name = columns$COLUMN_NAME,
    type = columns$TYPE_NAME,
    stringsAsFactors = FALSE)
  res
}

.odbcPreviewObject <- function(connection, rowLimit, table = NULL, view = NULL,
                                             schema = NULL, catalog = NULL, ...) {
  # extract object name from arguments
  name <- .validateObjectName(table, view)

  # check schema
  if (is.null(schema)) {
    stop("Schema not defined.")
  }
  retVal = exa.readData(connection, paste0("SELECT * FROM ", schema, ".", name, " LIMIT ", rowLimit))
  retVal
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

.build_code <- function(connection) {
  if (is.na(connection@db_name) | connection@db_name == "") {
    code <-
      c( paste0("library(", packageName(), ")"),
         paste0("con <- dbConnect(\"exa\", exahost= \"", connection@db_host, ":",
                connection@db_port, "\", uid=\"", connection@db_user, "\", pwd=<PASSWORD>, encryption=\"", ifelse(connection@encrypted, "Y", "N"),
                "\", schema=\"", connection@current_schema, "\")"))
  } else {
    code <-
      c( paste0("library(", packageName(), ")"),
         paste0("con <- dbConnect(", connection@db_name, ")"))
  }
  paste(code, collapse = "\n")
}

.on_connection_opened <- function(connection) {
  # make sure we have an observer
  observer <- getOption("connectionObserver")
  if (!is.null(observer)) {
    # let observer know that connection has opened
    observer$connectionOpened(
      # connection type
      type = .dbms_type(),

      # name displayed in connection pane
      displayName = .computeDisplayName(connection),

      # host key
      host = .computeHostName(connection),

      # icon for connection
      icon = .exasol_icon(),

      # connection code
      connectCode = .build_code(connection),

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
