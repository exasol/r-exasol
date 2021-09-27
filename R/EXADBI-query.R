#' @include EXADBI-connection.R
NULL

## Query related methods of the DBI API.

#' @title dbSendQuery
#' @describeIn dbSendQuery Sends an SQL statment to an EXASOL DB, prepares for result fetching.
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
      err <- append(err,sqlQuery(con, "alter session set profile='ON'"))
    }

    dbBegin(con)
    on.exit(dbEnd(con,commit = FALSE))

    if (stmt_cmd == "SELECT") {# ---------------if select ----------------------------------------
      temp_schema <- FALSE
      tbl_name <-
        paste0("TEMP_",floor(rnorm(1,1000,100) ^ 2),"_CREATED_BY_R")
      # con <- dbConnect(con, autocommit="N",...) # clone the connection with autocommit=off

      ids <- .EXAGetIdentifier(stmt, statement = TRUE)

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

#' @title fetch
#'
#' @describeIn fetch Fetches a subset of an result set.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @param res An EXAResult object.
#' @param n An int declaring the size of the subset to fetch. If missing, the whole subset is fetched.
#' @param ... further arguments to be passed on to exa.readData.
#' @include EXADBI-result.R
#' @seealso \code{\link[DBI:fetch]{DBI::fetch()}}
#' @export
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
setMethod(
  "fetch",signature(res = "EXAResult", n = "missing"),
  definition = function(res,...)
    .EXAFetch(res,...)
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
    #print("\n--------------\n")
    #print(df)
    #print("\n--------------\n")
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
setMethod(
  "dbClearResult", signature(res = "EXAResult"),
  definition = function(res,...)
    .EXAClearResult(res,...)
)

.EXAClearResult <- function(res,...) {
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

#' @title dbGetQuery
#' @describeIn dbGetQuery Executes the query, fetches and returns the entire result set.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn An EXAConnection object.
#' @param statement An SQL query statement to be executed in an EXASOL DB.
#' @param ... further arguments to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
#' @export
#' @seealso \code{\link[DBI:dbGetQuery]{DBI::dbGetQuery()}}
setMethod(
  "dbGetQuery", signature("EXAConnection","character"),
  definition = function(conn, statement,...) {
    if (.isSelectStatement(statement)) {
      return(exa.readData(conn,statement,...))
    } else {
      sqlQuery(conn, statement, errors = TRUE)
    }
  }
)
