#' @include EXADBI-connection.R
#' @include EXADBI-result.R
NULL

## Table related methods declared by DBI API.

#' @title dbListFields
#' @describeIn dbListFields Lists all fields of a table.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn An EXAConnection object
#' @param name A fully qualified table name in the form schema.table.
#' @param schema Filter on DB schema
#' @param ... further parameters passed on to `exa.readData()`
#' @return a character vector. If no tables present, a character vector of length 0.
#' @export
#' @seealso \code{\link[DBI:dbListFields]{DBI::dbListFields()}}
#' @include EXADBI-connection.R
setMethod(
  "dbListFields", signature("EXAConnection", "character"),
  definition = function(conn, name, schema, ...) {

    if (missing(schema)) {
      ids <- .EXAGetIdentifier(name, statement = FALSE)
      # try to grep schema from stmt
      if (length(ids)>0) {
        schema <- ids[[length(ids)]][1]
        name <- ids[[length(ids)]][2]
      }
      if (schema != "" & schema != "\"\"") {
        # message(paste("Using Schema from statement:", schema))
      } else {
        # message(paste("Using connection schema: ", con@current_schema))
        schema <- conn@current_schema
      }
    }
    schema <- processIDs(schema, quotes = "'")
    name <- processIDs(name, quotes = "'")

    qstr <- paste0("select column_name from exa_all_columns where column_schema = ", schema, " and
                   column_table = ", name, " order by column_ordinal_position")
    res <- exa.readData(conn, qstr, ...)
    return(res$COLUMN_NAME)
  })

#' @describeIn dbListFields Lists all fields of a result.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @param conn An EXAResult object
#' @param ... not used.
#' @return a character vector. If no tables present, a character vector of length 0.
#' @export
#' @include EXADBI-result.R
setMethod("dbListFields", signature("EXAResult"),
         definition = function(conn, ...) {
           conn$columns$COLUMN_NAME
         })

#' @title dbListTables
#' @describeIn dbListTables Lists all tables in the DB.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn An EXAConnection object
#' @param schema Filter on DB schema
#' @param ... further parameters passed on to `exa.readData()`
#' @return a character vector. If no tables present, a character vector of length 0.
#' @export
#' @seealso \code{\link[DBI:dbListTables]{DBI::dbListTables()}}
setMethod(
  "dbListTables", signature("EXAConnection"),
  definition = function(conn, schema, ...) {

    qstr <-
      paste0("select table_schema, table_name from exa_all_tables ", ifelse(!missing(schema), paste("where table_schema =",
                                                                                                    processIDs(schema,"'")), ""), " order by 1,2" )
    res <- exa.readData(conn, qstr, ...)
    return(paste0(res$TABLE_SCHEMA, ".", res$TABLE_NAME))
  })

#' @title dbReadTable
#' @describeIn dbReadTable Reads a DB table.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name in the form schema.table.
#' @param schema Alternatively and with preference to `name`, a schema can be specified separately.
#' @param order_col A string containing columns to have the result set ordered upon, e.g. "col1 desc, col2"
#' @param limit A row limit to the result set.
#'        CAUTION: a limit without order clause is non-deterministic in EXASOL,
#'        i.e. subsequent runs of the same statement may deliver differing result sets.
#' @param ... further arguements to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
#' @seealso \code{\link[DBI:dbReadTable]{DBI::dbReadTable()}}
#' @export
setMethod(
  "dbReadTable",
  signature("EXAConnection", "character"),
  definition = function(conn, name, schema = "", order_col = NA, limit = NA, ...) {
    .EXAReadTable(conn, name, schema, order_col, limit, ...)
  }
)

.EXAReadTable <- function(conn, name, schema, order_col, limit, ...) {
  if (schema == "") {
    ids <- .EXAGetIdentifier(name)
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


#' @title dbExistsTable
#' @describeIn dbExistsTable Checks if a table exists in an EXASOL DB.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name in the form schema.table.
#' @param schema Alternatively to `name`, a schema can be specified separately.
#' @return A logical indicating if the table exists.
#' @seealso \code{\link[DBI:dbExistsTable]{DBI::dbExistsTable()}}
#' @export
setMethod(
  "dbExistsTable", signature("EXAConnection", "character"),
  definition = function(conn, name, schema = "") {
    .EXAExistsTable(conn, name, schema)
  }
)

.EXAExistsTable <- function(conn, name, schema) {
  if (schema == "") {
    ids <- .EXAGetIdentifier(name, quotes = "'")
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

#' @title dbWriteTable
#' @describeIn dbWriteTable Writes a data.frame into a table. If the table does not exist, it is created.
#' @family EXAConnection related objects
#' @family DML functions
#'
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
#' @seealso \code{\link[DBI:dbWriteTable]{DBI::dbWriteTable()}}
#' @export
setMethod(
  "dbWriteTable", signature("EXAConnection", "character", "data.frame"),
  definition = function(conn, name, value, schema = "", field_types, overwrite = FALSE, writeCols = NA, ...) {
    .EXAWriteTable(conn, name, value, schema, field_types, overwrite, writeCols, ...)
  }
)

.EXAWriteTable <-
  function(con, tbl_name, data, schema, field_types, overwrite, writeCols, ...) {
    if (schema == "") {
      ids <- .EXAGetIdentifier(tbl_name)
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

    if (!is.na(writeCols)) {
      if (writeCols[1] == FALSE) {
        writeCols <- NA
      } # if write cols are missing or NA, write w/o specifying col names.
      else if (writeCols[1] == TRUE) {
        writeCols <- names(data)
      } # if TRUE, use the data.frame colnames, else use whatever is in it
    }
    message("Writing into table...")
    if (exa.writeData(con, data, paste0(schema,".",tbl_name),
                      tableColumns = processIDs(writeCols),...)) {
      on.exit(dbEnd(con))
      return(TRUE)
    }
    return(FALSE)
  }

#' @title dbRemoveTable
#' @describeIn dbRemoveTable Removes a table.
#' @family EXAConnection related objects
#' @family DML functions
#'
#' @param conn An EXAConnection object.
#' @param name a fully qualified table identifier (schema.table) or a simple table identifier, if the
#' parameter schema is specified.
#' @param schema An optional string specifying the DB schema. Caution: use either this parameter, OR give
#'      a fully qualified identifier as giving both will result in an error.
#' @param cascade A logical indicating whether also foreign key constraints referencing the table to
#'      be deleted shall be removed. Default is FALSE.
#' @return A logicl indicating success.
#' @seealso \code{\link[DBI:dbRemoveTable]{DBI::dbRemoveTable()}}
#' @author EXASOL AG <opensource@exasol.com>
#' @export
setMethod(
  "dbRemoveTable", signature("EXAConnection", "character"),
  definition = function(conn, name, schema = "", cascade = FALSE) {
    .EXARemoveTable(conn, name, schema, cascade)
  }
)

.EXARemoveTable <- function(con, tbl_name, schema, cascade) {
  if (schema == "") {
    ids <- .EXAGetIdentifier(tbl_name)
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
