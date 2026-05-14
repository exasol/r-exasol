#' @include EXADBI-connection.R
NULL

## Query related methods of the DBI API.

## Internal helper: execute SQL via WebSocket, returning the result list.
## On error, raises an R error (if errors=TRUE) or returns -1 (if errors=FALSE).
.wsExecuteQuery <- function(con, sql, errors = TRUE) {
  tryCatch({
    exaWsExecute(con@ws_handle, sql)
  }, error = function(e) {
    if (errors) stop(e)
    return(-1)
  })
}

#' @title dbSendQuery
#' @describeIn dbSendQuery Sends an SQL statement to an EXASOL DB, prepares for result fetching.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn A valid EXAConnection
#' @param statement vector mode character : an SQL statement to be executed in EXASOL db
#' @param schema vector mode character : a focus schema. This must have write access for the result
#'      set to be temporarily stored. If the user has only read permission on the schema to be read,
#'      another schema may be entered here, and table identifiers in stmt parameter must be
#'      fully qualified (schema.table).
#' @param profile logical, default TRUE : collect profiling information
#' @param default_fetch_rec numeric, default 100 :
#' @param ... unused.
#' @return EXAResult object which can be used for fetching rows. It also contains metadata.
#' @seealso \code{\link[DBI:dbSendQuery]{DBI::dbSendQuery()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbSendQuery",
  signature(conn = "EXAConnection", statement = "character"),
  definition = function(conn,
                        statement,
                        schema = "",
                        profile = TRUE,
                        default_fetch_rec = 100,
                        ...)
    .EXAExecStatement(
      con = conn,
      stmt = statement,
      schema = schema,
      profile = profile,
      default_fetch_rec = default_fetch_rec,
      ... = ...
    ),
  valueClass = "EXAResult"
)

.isSelectStatement <-
  function(statement) {
    return(grepl("^\\s*(\\/\\*.*\\*\\/)?\\s*(WITH.*)?SELECT",toupper(statement),perl=TRUE))
  }

.EXAExecStatement <-
  function(con, stmt, schema = "", profile = TRUE, default_fetch_rec = 100, ...) {
    if (.isSelectStatement(stmt)){
      stmt_cmd <- "SELECT"
    } else {
      stmt_cmd <- "NOSELECT"
    }
    qtime <- Sys.time()
    err <- vector(mode = "character")

    if (profile) {
      .wsExecuteQuery(con, "alter session set profile='ON'", errors = FALSE)
    }

    dbBegin(con)
    on.exit(dbEnd(con, commit = FALSE))

    if (stmt_cmd == "SELECT") {
      temp_schema <- FALSE
      tbl_name <-
        paste0("TEMP_", floor(rnorm(1, 1000, 100) ^ 2), "_CREATED_BY_R")

      ids <- .EXAGetIdentifier(stmt, statement = TRUE)

      if (schema == "") {
        if (length(ids) > 0) schema <- ids[[length(ids)]][1]
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
        schema <- tbl_name
        temp_schema <- TRUE
        err <- append(err, paste("Using temporary schema:", schema))
        message(paste("Using temporary schema:", schema))
      }
      schema <- processIDs(schema)

      if (temp_schema)
        .wsExecuteQuery(con, paste("create schema", schema))

      sq1 <- paste0("create table ", schema, ".", tbl_name, " as (", stmt, ")")

      errr <- .wsExecuteQuery(con, sq1, errors = FALSE)

      if (identical(errr, -1)) {
        warning("Failed to create temp result table.")
        err <- append(err, "Failed to create temp result table.")
      } else {
        dbEnd(con, commit = TRUE)
      }

    } else {
      if (schema != "") {
        schema <- processIDs(schema)
        err1 <- .wsExecuteQuery(con, paste("open schema", schema), errors = FALSE)
        if (identical(err1, -1)) {
          warning(paste("Schema cannot be opened:", schema))
          err <- append(err, paste("Schema cannot be opened:", schema))
        }
      }

      err2 <- .wsExecuteQuery(con, stmt, errors = FALSE)

      if (identical(err2, -1)) {
        stop("Query failed.")
      } else {
        dbEnd(con, commit = TRUE)
      }
    }

    .wsExecuteQuery(con, "flush statistics", errors = FALSE)

    if (stmt_cmd == "SELECT") {
      rc <- tryCatch({
        count_res <- .wsExecuteQuery(con, paste0("select count(*) from ", schema, ".", tbl_name))
        if (!is.null(count_res$data) && length(count_res$data) > 0) {
          count_res$data[[1]][1]
        } else 0
      }, error = function(e) 0)
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
    )

    cols <- data.frame()

    if (stmt_cmd == "SELECT") {
      if (!identical(errr, -1)) {
        message(rowcount, " rows prepared in ", sum(p$DURATION), " seconds.")
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
          where column_schema = ", processIDs(schema, quotes = "'"),
          " and column_table = ", processIDs(tbl_name, quotes = "'")
        )
      )

      res_tbl <- paste0(schema, ".", tbl_name)
    } else {
      res_tbl <- ""
    }

    EXAResult$new(
      connection = con,
      statement = stmt,
      rows_fetched = 0,
      rows_affected = rowcount,
      is_complete = ifelse(stmt_cmd == "SELECT", FALSE, TRUE),
      with_output = ifelse(stmt_cmd == "SELECT", TRUE, FALSE),
      profile = p,
      columns = cols,
      temp_result_tbl = res_tbl,
      query_sent_time = qtime,
      errors = err,
      default_fetch_rec = default_fetch_rec
    )
  }

#' @title fetch
#'
#' @describeIn fetch Fetches a subset of an result set.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @param res An EXAResult object.
#' @param n An int declaring the size of the subset to fetch. If missing, the whole subset is fetched.
#' @param ... further arguments to be passed on to fetch.
#' @include EXADBI-result.R
#' @seealso \code{\link[DBI:fetch]{DBI::fetch()}}
#' @export
#' @author EXASOL AG <opensource@exasol.com>
setMethod(
  "fetch", signature(res = "EXAResult", n = "numeric"),
  definition = function(res,n,...)
    .EXAFetch(res,n,...)
)

#' @describeIn fetch Fetches a subset of an result set without indicated the size of the subset to fetch.
#' @family EXAResult related objects
#' @family DQL functions
#' @param res An EXAResult object.
#' @export
#' @author EXASOL AG <opensource@exasol.com>
setMethod(
  "fetch",signature(res = "EXAResult", n = "missing"),
  definition = function(res,...)
    .EXAFetch(res,...)
)

#' @title dbFetch
#'
#' @describeIn fetch Fetches a subset of an result set.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @param res An EXAResult object.
#' @param n An int declaring the size of the subset to fetch. If missing, the whole subset is fetched.
#' @param ... further arguments to be passed on to fetch.
#' @include EXADBI-result.R
#' @seealso \code{\link[DBI:dbFetch]{DBI::dbFetch()}}
#' @export
#' @author EXASOL AG <opensource@exasol.com>
setMethod("dbFetch", signature("EXAResult"),
          definition = function(res, n = -1, ...)
            .EXAFetch(res, n, ...)
)

.EXAFetch <- function(res, n = res$default_fetch_rec, ...) {
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
    return(df)
  } else {
    warning("Fetch: No more to fetch.")
    return(data.frame())
  }
}

#' @title dbClearResult
#'
#' @describeIn dbClearResult Frees all resources associated with an \code{EXAResult}.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @param res An EXAResult object.
#' @param ... Further arguments to passed to res$close(). This may be 'commit=TRUE' (not advisable).
#' @return A logical indicating success.
#' @export
#' @seealso \code{\link[DBI:dbClearResult]{DBI::dbClearResult()}}
#' @author EXASOL AG <opensource@exasol.com>
setMethod(
  "dbClearResult", signature(res = "EXAResult"),
  definition = function(res,...)
    .EXAClearResult(res,...)
)

.EXAClearResult <- function(res, ...) {
  if (res$temp_result_tbl == "CLEARED") {
    message("Clear result: already cleared.")
    return(TRUE)
  }
  if (!res$with_output || res$temp_result_tbl == "") {
    message("No result set to clear.")
    res$temp_result_tbl <- "CLEARED"
    return(TRUE)
  } else {
    tryCatch(
      .wsExecuteQuery(res$connection, paste("drop table", res$temp_result_tbl)),
      error = function(e) {
        stop(paste("Couldn't remove temporary table. Delete:", res$temp_result_tbl))
      }
    )
    stbl <- strsplit(res$temp_result_tbl, ".", fixed = TRUE)
    if (stbl[[1]][1] == stbl[[1]][2] &&
        gregexpr("CREATED_BY_R", stbl[[1]][1])[[1]][1] > 0) {
      tryCatch(
        .wsExecuteQuery(res$connection, paste("drop schema", stbl[[1]][1])),
        error = function(e) {
          stop(paste("Couldn't remove temp. schema:", stbl[[1]][1]))
        }
      )
    }
    res$temp_result_tbl <- "CLEARED"
    return(TRUE)
  }
}

#' @title dbGetQuery
#' @describeIn dbGetQuery Executes the query, fetches and returns the entire result set.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn An EXAConnection object.
#' @param statement An SQL query statement to be executed in an EXASOL DB.
#' @param ... Further arguments to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
#' @export
#' @seealso \code{\link[DBI:dbGetQuery]{DBI::dbGetQuery()}}
#' @author EXASOL AG <opensource@exasol.com>
setMethod(
  "dbGetQuery", signature("EXAConnection", "character"),
  definition = function(conn, statement, ...) {
    if (.isSelectStatement(statement)) {
      return(exa.readData(conn, statement, ...))
    } else {
      .wsExecuteQuery(conn, statement)
    }
  }
)
