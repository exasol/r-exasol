#' @include EXADBI-object.R
#' @include EXADBI-driver.R
#' @include EXADBI-result.R
#' @include EXADBI-connection.R
NULL

#--------------------- dbGetInfo --------------------------

#' @title dbGetInfo
#' @describeIn dbGetInfo Returns metadata on a given EXAObject.
#'
#' @param dbObj An EXAObject.
#' @return A named list.
#' @export
#' @seealso \code{\link[DBI:dbGetInfo]{DBI::dbGetInfo()}}
setMethod(
  "dbGetInfo", "EXAObject",
  definition = function(dbObj) {
    return("EXASOL DBI Object.")
  }
)

#' @describeIn dbGetInfo Returns metadata on a given EXADriver.
#'
#' @param dbObj Existing and valid EXADriver object.
#' @export
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

#' @describeIn dbGetInfo Returns metadata on a given EXAConnection.
#'
#' @param dbObj Existing and valid EXAConnection object.
#' @export
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

#' @describeIn dbGetInfo Returns metadata on a given EXAResult.
#'
#' @param dbObj existing EXAResult object.
#' @export
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
