#' @include EXADBI.R

## These are methods providing compatibility to dplyr.
##
## First version written in 2016 by Marcel Boldt <marcel.boldt@exasol.com>
## as part of the EXASOL R interface & SDK package. It may be used, changed and distributed freely
## with no further restrictions than already stipulated in the package license, with the exception
## that this statement must stay included and unchanged.


#' @export
src_exasol <- function(host, port = 8563, user = "sys", password = "exasol", ..., dsn = NULL) {

  if (!require(dplyr))
    stop("Please install dplyr: install.packages('dplyr')")
  if (!missing(dsn)) {
    con <- dbConnect("exa", dsn=dsn, ... = ...)
  } else {
  if (missing(host))
    stop("No host specified. Please provide a valid IP or URL.")

  con <- dbConnect("exa", exahost = paste0(host,":",port), uid = user, pwd = password, ... = ...)
}
  src_sql("exasol", con)
}


#' @export
src_desc.src_exasol <- function(con) {
  info <- dbGetInfo(con$con)
  host <- if (info$host == "") "localhost" else info$host

  paste0("EXASOL ", info$db.version, " [", info$username, "@",
         host, ":", info$port, "]")
}


db_list_tables.src_exasol <- function(con, schema, ...) {
  dbListTables(con$con, schema, ... = ...)
}


#' @export
db_has_table.src_exasol <- function(con, table, ...) {
  dbExistsTable(con$con, table, ...)
}


#' @export
tbl.src_exasol <- function(src, from, schema = NULL,  ...) {

  if (missing(schema)) {
    ids <- EXAGetIdentifier(from, quotes="")
    schema <- ids[[1]][1]
    tbl_name <- ids[[1]][2]
  } else {
    schema <- processIDs(schema)
    tbl_name <- processIDs(tbl_name, quotes="")
  }
  if(schema == "") {
    schema <- src$con@current_schema
  } else {
 #   if (!missing(schema)) {
      dbCurrentSchema(src$con, setSchema = schema)
 #   }
  }

  tbl_sql("exasol", src = src, from = ident(tbl_name), ... = ...)
}


#' @export
src_translate_env.src_exasol <- function(x) {
  sql_variant(
    base_scalar,
    sql_translator(.parent = base_agg,
                   n = function() sql("count(*)"),
                   cor = sql_prefix("corr"),
                   cov = sql_prefix("covar_samp"),
                   sd =  sql_prefix("stddev_samp"),
                   var = sql_prefix("var_samp"),
                   all = sql_prefix("bool_and"),
                   any = sql_prefix("bool_or"),
                   paste = function(x, collapse) build_sql("string_agg(", x, ", ", collapse, ")")
    ),
    base_win
  )
}

#' @export
db_explain.EXAConnection <- function(con, ...) {
  stop("EXPLAIN statement is currently unsupported.")
}


#' @export
copy_to.src_exasol <- function(dest, df, name = deparse(substitute(df)), ...) {
  # TODO: tbl/schema identifier handling as in src_exasol
  df<- as.data.frame(df)
  dbWriteTable(dest$con, name, df, ...)
  tbl(dest, name)
}
