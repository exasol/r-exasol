.odbcListObjectTypes <- function(connection) {
  # slurp all the objects in the database so we can determine the correct
  # object hierarchy

  # all databases contain tables, at a minimum
  obj_types <- list(table = list(contains = "data"))

  # see if we have views too
  views_df <- exa.readData(connection, "SELECT * FROM EXA_ALL_VIEWS")
  if (nrow(views_df) >0) {
    obj_types <- c(obj_types, list(view = list(contains = "data")))
  }

  obj_types <- c(obj_types, list(db_function = list(contains = "data")))

  # assume we always have schemas!
  obj_types <- list(schema = list(contains = obj_types))

  #
  # # check for multiple catalogs
  # catalogs <- string_values(connection_sql_tables(connection@ptr, "%", "", "", "")[["table_catalog"]])
  # if (length(catalogs) > 0) {
  #   obj_types <- list(catalog = list(contains = obj_types))
  # }

  obj_types
}

.list_table_names <- function(connection, schema, name = NULL) {
  q <- paste0("SELECT TABLE_NAME FROM EXA_ALL_TABLES WHERE TABLE_SCHEMA=", processIDs(schema, quotes="'"))
  if (!is.null(name)) {
    q <- paste0(q, " AND TABLE_NAME=", processIDs(name, quotes = "'"))
  }
  tables_df <- exa.readData(connection, q)

  data.frame(
    name = tables_df$TABLE_NAME,
    type = rep("table", nrow(tables_df)),
    stringsAsFactors = FALSE
  )
}

.list_view_names <- function(connection, schema, name = NULL) {
  q <- paste0("SELECT VIEW_NAME FROM EXA_ALL_VIEWS WHERE VIEW_SCHEMA=", processIDs(schema, quotes="'"))
  if (!is.null(name)) {
    q <- paste0(q, " AND VIEW_NAME=", processIDs(name, quotes = "'"))
  }
  views_df <- exa.readData(connection, q)

  data.frame(
    name = views_df$VIEW_NAME,
    type = rep("view", nrow(views_df)),
    stringsAsFactors = FALSE
  )
}

.list_function_names <- function(connection, schema, name = NULL) {
  q <- paste0("SELECT FUNCTION_NAME FROM EXA_ALL_FUNCTIONS WHERE FUNCTION_SCHEMA=", processIDs(schema, quotes="'"))
  if (!is.null(name)) {
    q <- paste0(q, " AND FUNCTION_NAME=", processIDs(name, quotes = "'"))
  }
  views_df <- exa.readData(connection, q)

  data.frame(
    name = c("TestFunc"),
    type = c("db_function"), #rep("db_function", nrow(views_df)),
    stringsAsFactors = FALSE
  )
}

odbcListObjects <- function(connection, catalog = NULL, schema = NULL, name = NULL, type = NULL, ...) {

  res <- data.frame()
  # if no schema was supplied, return a list of schema
  warning(paste0("odbcListObjects called with(schema=", schema, " name=", name, " type=", type, ")\n"))
  if (is.null(schema)) {
    schemas_df <- exa.readData(connection, "SELECT SCHEMA_NAME FROM EXA_ALL_SCHEMAS")
    if (nrow(schemas_df) > 0) {
      res <-
        data.frame(
          name = schemas_df$SCHEMA_NAME,
          type = rep("schema", times = nrow(schemas_df)),
          stringsAsFactors = FALSE
      )
    }
  }
  else if (is.null(type)){
    tables_df <- .list_table_names(connection, schema, name)
    views_df <- .list_view_names(connection, schema, name)
    functions_df <- .list_function_names(connection, schema, name)
    res <- rbind(tables_df, views_df, functions_df)
  }
  else if (tolower(type) == "table") {
    res <- .list_table_names(connection, schema, name)
  }
  else if (tolower(type) == "view") {
    res <- .list_view_names(connection, schema, name)
  }
  else if (tolower(type) == "db_function") {
    res <- .list_function_names(connection, schema, name)
  }
  res
}

# given a connection, returns its "host name" (a unique string which identifies it)
.computeHostName <- function(connection) {
  connection@db_host
}

.computeDisplayName <- function(connection) {
  connection_name <- connection@db_name
  if (is.na(connection_name)) {
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
  .validateObjectName(table, view)

  if(!is.null(table)) {
    q <-  paste0("SELECT COLUMN_NAME, COLUMN_TYPE FROM EXA_ALL_COLUMNS WHERE COLUMN_SCHEMA=",
                 processIDs(schema, quotes="'") , " AND COLUMN_TABLE=",  processIDs(table, quotes="'"),
                 " AND COLUMN_OBJECT_TYPE='TABLE'")
    columns_df <- exa.readData(connection, q)
    res <- data.frame(
      name = columns_df$COLUMN_NAME,
      type = columns_df$COLUMN_TYPE,
      stringsAsFactors = FALSE)
  } else {
    columns_df <- exa.readData(connection, paste0("SELECT COLUMN_NAME, COLUMN_TYPE FROM EXA_ALL_COLUMNS WHERE COLUMN_SCHEMA=",
                                                  processIDs(schema, quotes="'") , " AND COLUMN_TABLE=",  processIDs(view, quotes="'"),
                                                  " AND COLUMN_OBJECT_TYPE='VIEW'"))
    res <- data.frame(
      name = columns_df$COLUMN_NAME,
      type = columns_df$COLUMN_TYPE,
      stringsAsFactors = FALSE)
  }
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
  #res <- dbSendQuery(connection, statement = paste0("SELECT * FROM ", schema, ".", name), default_fetch_rec = rowLimit)
  #retVal <- dbFetch(res, rowLimit)
  #dbClearResult(res)
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

.build_code <- function() {
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
        odbcListObjects(connection, ...)
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
