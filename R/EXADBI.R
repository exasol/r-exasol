
## These are the DBI related classes and methods, which work as an abstraction layer over the basic exa functions
## and provide compabibility to the DBI package. The structure may also serve as a foundation for a later implemntation
## of a proprietary CLI interface that does not depend on RODBC.
##
## First version written in 2015 by Marcel Boldt <marcel.boldt@exasol.com>
## as part of the EXASOL R interface & SDK package. It may be used, changed and distributed freely with no further restrictions than
## already stipulated in the package license, with the exception that this statement must stay included and unchanged.

#' @export dbDriver
#' @export dbUnloadDriver
#' @export dbConnect
#' @export dbListConnections

#' @export dbDisconnect
#' @export dbSendQuery
#' @export dbGetQuery
#' @export dbGetException
#' @export dbListResults
#' @export dbListFields
#' @export dbListTables
#' @export dbReadTable
#' @export dbWriteTable
#' @export dbExistsTable
#' @export dbRemoveTable
#' @export dbBegin
#' @export dbCommit
#' @export dbRollback

#' @export dbFetch
#' @export dbClearResult
#' @export dbColumnInfo
#' @export dbGetStatement
#' @export dbHasCompleted
#' @export dbGetRowsAffected
#' @export dbGetRowCount
#' Class definitions -------------------------------------------------------------------------------
 #' The virtual object constituting a basis to all other EXA DBI Objects.
 #' @family DBI classes
 setClass("EXAObject", contains = c("DBIObject", "VIRTUAL"))

setOldClass("RODBC") # the S3 class RODBC will be registered as a superclass of EXAConnection

 #' An interface driver object to the EXASOL Database.
 #' @family DBI classes
 #' @family EXADriver related objects
 setClass("EXADriver", contains = c("DBIDriver", "EXAObject"))


#' An Object holding a connection to an EXASOL Database.
#' @family DBI classes
#' @family EXAConnection related objects
#'
#' @slot init_connection_string A string containing the ODBC connection sting used to initialise the connection.
#' @slot current_schema A string reflecting the current schema.
#' @slot autocom_default A logical indicating if autocommit is active.
#' @slot connection.string RODBC
#' @slot handle_ptr RODBC
#' @slot case RODBC
#' @slot id RODBC
#' @slot believedNRows RODBC
#' @slot colQuote RODBC
#' @slot tabQuote RODBC
#' @slot encoding RODBC
#' @slot rows_at_time RODBC
#' @export
setClass("EXAConnection",
                              slots = c(init_connection_string="character",
                                        current_schema="character",
                                        autocom_default="logical"
                                        ),
                            contains = c("DBIConnection", "EXAObject","RODBC")
)



#' An object that is associated with a result set in an EXASOL Database.
#' @family DBI classes
#' @family EXAResult related objects
#'
#' @field connection An EXAConnection object.
#' @field statement A string containing the SQL query statement.
#' @field rows_fetched An int reflecting the rows already fetched from the DB.
#' @field rows_affected An int reflecting the length of the dataset in the DB.
#' @field is_complete A logical indcating if the query processing by the DB has been completed.
#' @field with_output A logical indicating whether the query produced a result set.
#' @field profile A data.frame containing profile information on the query.
#' @field columns A data.frame containing column metadata.
#' @field temp_result_tbl A string reflecting the name of the (temporary) table that holds the result set.
#' @field query_sent_time A POSIXct timestamp indicating when the query was sent to the DB.
#' @field errors A character vector containing errors.
#' @field default_fetch_rec An int reflecting the default fetch size.
#' @export
EXAResult <- setRefClass("EXAResult",
                         fields = c(connection="EXAConnection",
                                statement="character",
                                rows_fetched="numeric",
                                rows_affected="numeric",
                                is_complete="logical",
                                with_output="logical",
                                profile="data.frame",
                                columns="data.frame",
                                temp_result_tbl="character",
                                query_sent_time="POSIXct",
                                errors="character",
                                default_fetch_rec="numeric"
                                ),
    contains = c("DBIResult", "EXAObject"),
    methods = list(
        refreshMetaData = function(x) {
            "Refreshes the object's metadata."
            print("todo")
        },
        addRowsFetched = function(x) {
            "Add an int (the length of a newly fetched result set) to rows_fetched."
            rows_fetched <<- rows_fetched + as.numeric(x)
        },
        close = function(commit=FALSE) {
            "Frees up all resources, in particular drops the temporary table in the DB."
            if(odbcEndTran(connection, commit)==0) {
                message("Changes commited.")
            } else stop("Commit failed. Changes NOT commited. Closing aborted.")

            message("Closing connection...")
            res <- try(odbcClose(connection),silent=TRUE)
            if(res==1) {
                message("Connection closed.")
                return(TRUE)
            } else if(res==0) {
                warning("Closing not successful.")
                return(FALSE)
            } else {
                    warning(res)
                    return(FALSE)
                }
        },
        finalize = function(...) {
            close()
            message("Object terminated.")
        }
    )
)



# Instantiates an EXADriver object.
# @family EXADriver related objects
#
# @return An EXADriver object.
exa <- function() {
    print("EXASOL driver loaded")
    new("EXADriver")
}

exasol <- function() {
  print("EXASOL driver loaded")
  new("EXADriver")
}

#' Checks if an EXAObject is still valid.
#'
#' @name dbIsValid
#' @param conn An object that inherits EXAObject.
#' @return A logical indicating if the connection still works.
setMethod("dbIsValid", signature("EXAObject"),
          def=function(dbObj) {
            return(TRUE) # TODO
          })


# Connection -------------------------------------------------------------------

#' Creates a connection to an EXASOL Database.
#' @family EXADriver related objects
#' @family EXAConnection related objects
#'
#' @name dbConnect
#' @param drv An EXAdriver object, a character string "exasol" or "exa", or an existing
#'        EXAConnection object (for connection cloning).
#' @param exahost DNS or IP and port of the database cluster, e.g. '10.0.2.15..20:8563'
#' @param uid DB username, e.g. 'sys'
#' @param pwd DB user password, e.g. 'exasol'
#' @param schema Schema in EXASOL db which is opened directly after the connection.
#' @param exalogfile the EXASOL odbc driver log file. By standard a tempfile is created. Log data may be
#'        accessed with 'EXAlog(EXAConnection)'.
#' @param logmode EXASOL ODBC driver log mode. By default, no log is written ('NONE'). Other options are
#'          'DEFAULT'           (most important function calls & SQL commands),
#'          'VERBOSE'           (also additional data about internal steps & result data),
#'          'ON ERROR ONLY'     (only errors are logged),
#'          'DEBUGCOMM'         (extended logs, similar to verbose but w/o data & parameter tables).
#' @param encryption ODBC encryption. By default off. Switch on with 'Y'.
#' @param autocommit By default 'N'. If Y' each SQL statement is committed. Off means that
#'     no commits are executed automatically. The transaction will be rolled back on disconnect, which
#'     causes the loss of all data written during the transaction. The methods in the EXASOL package
#'     handle their transaction themselves, but but if the connection is used otherwise it might be advisable to
#'     switch autocommit to 'Y'.
#' @param querytimeout Time EXASOL DB computes a query before it is aborted. The default '0' (zero) means
#'      no timeout, i.e. runs until finished.
#' @param connectionlcctype Sets the connection locale LC CTYPE. The default is the setting of the client's current R session.
#' @param connectionlcnumeric Sets the connection locale LC NUMERIC. The default is the setting of the client's current R session.
#' @param encoding The connection encoding. TODO.
#' @param ... Additional parameters to the connection string. If a connection is cloned, these override the old connection settings.
#' @param dsn A preconfigured ODBC Data Source Name. Parameter being evaluated with priority to EXAHOST.
#' @param connection_string alternatively to everything else, a custom ODBC connection sting can be
#'      provided. See EXASOL DB manual secion 4.2.5 for details, available at \url{https://www.exasol.com/portal}.
#' @return A fresh EXAConnection object.
#' @examples \dontrun{
#' con <- dbConnect("exa",dsn="EXASolo")
#'
#' con <- dbConnect("exa",exahost="212.209.123.20..25:8563",uid="peter",pwd="password123",schema="sales")
#' }
setMethod("dbConnect", "EXADriver",
          def = function(    drv, # change defaults also below
                             exahost="",
                             uid="",
                             pwd="",
                             schema="SYS",
                             exalogfile=tempfile(pattern="EXAODBC_",fileext = ".log"),
                             logmode="NONE",
                             encryption="N",
                             autocommit="N",
                             querytimeout="0",
                             connectionlcctype=Sys.getlocale(category="LC_CTYPE"),
                             connectionlcnumeric=Sys.getlocale(category="LC_NUMERIC"),
                             ...,
                             dsn="",
                             connection_string="")
          EXANewConnection(drv,exahost, uid, pwd,schema, exalogfile, logmode, encryption, autocommit,
                           querytimeout, connectionlcctype, connectionlcnumeric, ..., dsn,connection_string),
          valueClass = "EXAConnection"
)


# @family EXADriver related objects
# @family EXAConnection related objects
#
# @inheritParams dbConnect
setMethod("dbConnect", "character",
          def = function(drv, ...) EXANewConnection(drv=dbDriver(drv), ...),
          valueClass = "EXAConnection"
)

# @family EXADriver related objects
# @family EXAConnection related objects
#
# @inheritParams dbConnect
setMethod("dbConnect", "EXAConnection",
          def = function(drv, ...) EXACloneConnection(drv, ...),
          valueClass = "EXAConnection"
)


#' Fetches and outputs the current schema from an EXASOL DB. Also updates EXAConnection metadata.
#' @family EXAConnection related objects
#'
#' @name dbCurrentSchema
#' @param con a valid EXAConnection
#' @return an updated EXAConnection
#' @export
dbCurrentSchema <- function(con) {

    res <- sqlQuery(con, "select current_schema")
    con@current_schema <- as.character(res[1,1])
    message(paste("Schema: ",con@current_schema))
    con

}



EXANewConnection <- function( # change defaults also above
    drv,
    exahost="",
    uid="",
    pwd="",
    schema="SYS",
    exalogfile=tempfile(pattern="EXAODBC_",fileext = ".log"),
    logmode="NONE",
    encryption="N",
    autocommit="N",
    querytimeout="0",
    connectionlcctype=Sys.getlocale(category="LC_CTYPE"),
    connectionlcnumeric=Sys.getlocale(category="LC_NUMERIC"),
    ...,
    dsn="",
    connection_string=""
) {
  exaschema <- c(schema)

    if(connection_string != ""){
        con_str <- connection_string
    }
    else {
        if(dsn!="") {
            con_str <- paste0("DSN=",dsn)
        }
        else if (exahost!="" & uid!="") {
            con_str <- paste0("DRIVER={EXASolution Driver};EXAHOST=",exahost)
        }
        else {
            stop("Connect failed. Either DSN, host & db_user or a connection string must be given.")
        }
        # all additional parameters...
        if(uid!="") {
            con_str <- paste0(con_str,";UID=",uid,";PWD=",pwd)
        }
        # EXASCHEMA
        if(exaschema!="SYS") {
            con_str <- paste0(con_str,";EXASCHEMA=",exaschema)
        }
        # EXALOGFILE
        #print(exalogfile)
        con_str <- paste0(con_str,";EXALOGFILE=",exalogfile)

        # LOGMODE
        con_str <- paste0(con_str,";LOGMODE=",logmode)

        # locale
        con_str <- paste0(con_str,";CONNECTIONLCCTYPE=",connectionlcctype,";CONNECTIONLCNUMERIC=",connectionlcnumeric)

        # autocommit

        con_str <- paste0(con_str,";autocommit=",autocommit)

        # dots
        d <- list(...)
        while (length(d)>0) {
            con_str <- paste0(con_str,";", names(d[1]),"=",d[1])
            d[1] <- NULL
        }
    }

    con <- odbcDriverConnect(con_str)
    new("EXAConnection",init_connection_string = con_str,
        current_schema=exaschema,
        autocom_default=ifelse(autocommit=="Y",TRUE,FALSE),
        con)
}

# Opens a new connection with the same settings as an existing one.
# @family EXADriver related objects
# @family EXAConnection related objects
#
# @param drv an EXAConnection object to be dublicated.
# @param ... additional connection string parameter that may override the old settings.
# @return a fresh EXAConnection
EXACloneConnection <- function(drv, autocommit, ...) { # todo: parameters

    drv <- dbCurrentSchema(drv) # update schema metadata

    # dots
    d <- data.frame(...,stringsAsFactors = FALSE)
    names(d) <- toupper(names(d))
    con_str <- drv@init_connection_string
    s <- strsplit(con_str, ";")
    s <- sapply(s, strsplit,"=")
    s <- lapply(s, function(x) toupper(x))

    con_str <- ""

    while (length(s)>0) { # as long as there is at least one parameter in S
        if(is.null(d[[s[[1]][1]]])) {          # if the first parameter of s (S is the conn_str) is not in d (the dots parameters)
            con_str <- paste0(con_str,";",s[[1]][1],"=",s[[1]][2])      # take the s parameter
            s[1] <- NULL # delete the first parameter from S

        } else {                                # else take the value out of d, delete the parameter from d, then delete the first S parameter
            con_str <- paste0(con_str,";",s[[1]][1],"=",d[[s[[1]][1]]])
            d[[s[[1]][1]]] <- NULL
            s[1] <- NULL
        }
    }                           # add the remaining dots parameters
    while (ncol(d)>0) {
        con_str <- paste0(con_str,";", names(d)[1],"=",d[[1]])
        d[1] <- NULL
    }


    con_str <- substr(con_str,2,nchar(con_str)) # remove the initial semicolon

    con <- odbcDriverConnect(con_str)
    if(con == -1) {
        stop(paste("EXACloneConnection error: failed to initialise connection.\nConnection String:",con_str))
    }
    new("EXAConnection",
        init_connection_string = con_str,
        current_schema=drv@current_schema,
        autocom_default=ifelse(!missing(autocommit),ifelse(autocommit=="Y",TRUE,FALSE),drv@autocom_default),
        con)
}

#' Sends a commit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbCommit
#' @param conn An EXAConnection object
#' @return a logical indicating success.
setMethod("dbCommit", signature("EXAConnection"),
          function(conn) {
              switch(as.character(odbcEndTran(conn,commit=TRUE)),
                     "-1" = {stop(paste0("Commit failed:\n",odbcGetErrMsg(conn)));return(FALSE)},
                     "0" = {message("Transaction committed.");return(TRUE)},
                     {    print("Commit failed.")
                         stop(odbcGetErrMsg(conn))
                         return(FALSE)
                     }
              )
          }
)

#' Rolls the current DB transaction back.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbRollback
#' @param conn An EXAConnection object
#' @return a logical indicating success.
setMethod("dbRollback", signature("EXAConnection"),
          function(conn) {
              switch(as.character(odbcEndTran(conn,commit=FALSE)),
                     "-1" = {stop(paste0("Rollback failed:\n",odbcGetErrMsg(conn)));return(FALSE)},
                     "0" = {message("Transaction rolled back.");return(TRUE)},
                     {    print("Rollback failed.")
                         stop(odbcGetErrMsg(conn))
                         return(FALSE)
                     }
              )
          }
)


#' Starts a DB transaction. In EXASOL, it disables autocommit.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbBegin
#' @param conn An EXAConnection object
#' @return a logical indicating success.
setMethod("dbBegin", signature("EXAConnection"),
          function(conn) {
            odbcSetAutoCommit(conn, autoCommit = FALSE)
            #message("Transaction started.")
              return(TRUE)
          }
)


#' Ends a DB transaction. In EXASOL, it commits and reinstates the connection's standard autocommit mode.
#' This is an EXASOL specific addition to the DBI interface and may not work with other RDBMS.
#'
#' @family EXAConnection related objects
#' @family transaction management functions
#'
#' @name dbEnd
#' @param conn An EXAConnection object
#' @param commit Logical. on TRUE the transaction is commmitted, otherwise rolled back.
#' @return a logical indicating success.
#' @export
setGeneric("dbEnd",
           def = function(conn,...) standardGeneric("dbEnd"),
           valueClass="logical"
)

setMethod("dbEnd", signature("EXAConnection"),
          function(conn,commit=TRUE) {
            ifelse(commit, dbCommit(conn), dbRollback(conn))
            odbcSetAutoCommit(conn, autoCommit = conn@autocom_default)
           # message("Transaction completed.")
            return(TRUE)
          }
)

#' Disconnects the connection.
#'
#' @name dbDisconnect
#' @param conn An EXAConnection object.
#' @return A logical indicating success.
setMethod("dbDisconnect",signature("EXAConnection"),
          def=function(conn) {
            odbcClose(conn)
          })



# Querying ----------------------------------------------------------------------------------------
#



#' Sends an SQL statment to an EXASOL DB, prepares for result fetching.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbSendQuery
#' @param con a valid EXAConnection
#' @param stmt vector mode character : an SQL statement to be executed in EXASOL db
#' @param schema vector mode character : a focus schema. This must have write access for the result set to be temporarily stored. If the user has only read permission on the schema to be read, another schema may be entered here, and table identifiers in stmt parameter must be fully qualified (schema.table).
#' @param profile logical, default TRUE : collect profiling information
#' @param default_fetch_req numeric, default 100 :
#' @param ... additional parameters to be passed on to dbConnect (used to clone the connection to one without autocommit)
#' @return EXAResult object which can be used for fetching rows. It also contains metadata.
setMethod("dbSendQuery",
          signature(conn = "EXAConnection", statement = "character"),
          def = function(conn,statement,...) EXAExecStatement(conn, statement,...),
          valueClass = "EXAResult"
)

# Tries to extrapolate the schema name from the statement given. It looks for strings framed by 'from' and a dot (not case sensitive).
#
# @param stmt The statement used
# @param alt Alternative that is returned if no schema is found. Default: 'R_temp'
# @return a character vector containing all schemas found, or alt
grep_schema <- function(stmt) {
    grep_a <- gregexpr("from\\s[\\w]+?\\.",stmt, perl=TRUE,ignore.case=TRUE)
    if(length(grep_a[[1]][1])==-1) {return("")}
    grep_b <- regmatches(stmt,grep_a)
    schema <- substring(grep_b[[1]],6,nchar(grep_b[[1]])-1)
    schema
}


EXAExecStatement <- function(con, stmt, schema="", profile=TRUE, default_fetch_rec=100,...) {

    qtime <- Sys.time()
    err <- vector(mode="character")
    temp_schema <- FALSE
    tbl_name <- paste0("TEMP_",floor(rnorm(1,1000,100)^2),"_CREATED_BY_R")
    con <- dbConnect(con, autocommit="N",...) # clone the connection with autocommit=off

    if(schema=="") { # try to grep schema from stmt
        s <- grep_schema(stmt)
        if(length(s)>1) {
            warning("Multiple schemas found in statement: ",s,"Using ",s[length(s)])

        }
        schema <- s[length(s)]
    }
    if(schema=="") { # if nothing helps use temp_schema
        schema <- tbl_name
        temp_schema <- TRUE
    }


    if(profile) {
        err <- append(err,sqlQuery(con, "alter session set profile='ON'"))
    }

    if (temp_schema) err <- append(err, sqlQuery(con, paste("create schema",schema)))
    errr <- sqlQuery(con,paste0("create table ",schema,".",tbl_name," as (",stmt,")")) # on success this won't return anything
    err <- append(err, errr)



    sqlQuery(con,"flush statistics")
    p <- exa.readData(con, "select
                      session_id,
                      stmt_id,
                      part_id,
                      object_name,
                      object_rows,
                      duration,
                      cpu,
                      temp_db_ram_peak,
                      hdd_read,
                      net
                      from exa_user_profile_last_day
                      where session_id = current_session and stmt_id=current_statement-2
                      order by part_id desc") # current_statement: -2 if autocommit=N, otherwise -4

    if (length(errr)==0) {
        message(p$OBJECT_ROWS[1]," rows prepared in ",sum(p$DURATION)," seconds.")
    } else {
        warning(errr)
    }

    cols <- exa.readData(con, paste0("select
                                     column_ordinal_position,
                                     column_name, column_comment,
                                     column_type, column_maxsize,
                                     column_is_nullable,
                                     column_default,
                                     column_identity,
                                     column_owner,
                                     column_is_distribution_key
                                     from exa_user_columns
                                     where column_schema = '",schema,"' and column_table = '",tbl_name,"'"))

    EXAResult$new(
        connection=con,
        statement=stmt,
        rows_fetched=0,
        rows_affected=as.numeric(p$OBJECT_ROWS[1]),
        is_complete=TRUE,
        with_output=TRUE,
        profile=p,
        columns=cols,
        temp_result_tbl=paste0(schema,".",tbl_name),
        query_sent_time=qtime,
        errors=err,
        default_fetch_rec=default_fetch_rec
    )
}

#' Fetches a subset of an result set.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @name dbFetch
#' @aliases fetch
#' @param res An EXAResult object.
#' @param n An int declaring the size of the subset to fetch. If missing, the whole subset is fetched.
#' @param ... further arguments to be passed on to exa.readData.
setMethod("fetch",signature(res="EXAResult",
                            n="numeric"),
          def=function(res,n,...) EXAFetch(res,n,...)
)

# @family EXAResult related objects
# @family DQL functions
#
# @inheritParams fetch
# @export
setMethod("fetch",signature(res="EXAResult",
                            n="missing"),
          def=function(res,...) EXAFetch(res,...)
)



EXAFetch <- function(res,n=res$default_fetch_rec,...) {

    if(n==-1) {
        n <- res$rows_affected
    }
    query <- paste("select * from",res$temp_result_tbl,"order by rownum limit",n,"offset",res$rows_fetched)
    df <- exa.readData(res$connection, query,...)
    res$rows_fetched <- res$addRowsFetched(nrow(df))
    df
}


#' Frees all resources associated with an \code{EXAResult}.
#' @family EXAResult related objects
#' @family DQL functions
#'
#' @name dbClearResult
#' @param res An EXAResult object.
#' @param ... Further arguments to passed to res$close(). This may be 'commit=TRUE' (not advisable).
#' @export
setMethod("dbClearResult", signature(res="EXAResult"),
          def=function(res,...) EXAClearResult(res,...)
)

EXAClearResult <- function(res,...) {
    res$close()
}

#' Executes the query, fetches and returns the entire result set.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbGetQuery
#' @param conn An EXAConnection object.
#' @param statement An SQL query statement to be executed in an EXASOL DB.
#' @param ... further arguements to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
setMethod("dbGetQuery", signature("EXAConnection","character"),
          def=function(conn, statement,...) {
              exa.readData(conn,statement,...)
          }
)

#' Reads a DB table.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbReadTable
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name in the form schema.table.
#' @param order_col A string containing columns to have the result set ordered upon, e.g. "col1 desc, col2"
#' @param limit A row limit to the result set.
#'        CAUTION: a limit without order clause is non-deterministic in EXASOL,
#'        i.e. subsequent runs of the same statement may deliver differing result sets.
#' @param ... further arguements to be passed on to exa.readData.
#' @return The result exa.readData, by default a data.frame containing the result set.
setMethod("dbReadTable", signature("EXAConnection","character"),
          def=function(conn, name,order_col=NA,limit=NA,...) {
              statement <- paste("select * from",name)
              if(!is.na(order_col)) {
                  statement <- paste(statement, "order by (",order_col,")")
              }
              if(!is.na(limit)) {
                  statement <- paste(statement,"limit",limit)
              }
              exa.readData(conn,statement,...)
          }
)

#' Changes an identifier into uppercase, except for it is quoted.
#'
#' @name EXAupper
#' @param identifier A character vector containing one or many (table, schema, column,...) identifiers.
#' @return A character vector containing one or many processed identifiers.
#' @export
EXAupper <- function(identifier) {
    for(i in 1:length(identifier))
    if(substr(identifier[i],1,1) == "\"" & substr(identifier[i],nchar(identifier[i]),nchar(identifier[i])) == "\"")
        identifier[i] <- identifier[i] # quoted
    else identifier[i] <- toupper(identifier[i]) # not quoted
    identifier
}

#' Checks if a table exists in an EXASOL DB.
#' @family EXAConnection related objects
#' @family DQL functions
#'
#' @name dbExistsTable
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name in the form schema.table.
#' @return A logical indicating if the table exists.
setMethod("dbExistsTable", signature("EXAConnection", "character"),
          def=function(conn,name) {
              n <- strsplit(name,".",fixed=TRUE)[[1]]
              if(length(n) != 2) stop("Error: Schema/table identifier ambiguous. Use SCHEMA.TABLE.")
              n <- EXAupper(n)
              qstr <- paste0("select * from exa_all_tables where table_schema = '",n[1],"' and table_name='",n[2],"'")
              res <- sqlQuery(con, qstr)
              if(nrow(res) == 0) {return(FALSE)}
              else if(nrow(res) == 1) {return(TRUE)}
              else if (nrow(res) > 1) {warning("Identifier ambiguous. Multiple matches.")}
              else {stop("Unknown error.")}
          }
)

#' Writes a data.frame into a table. If the table does not exist, it is created.
#' @family EXAConnection related objects
#' @family DML functions
#'
#' @name dbWriteTable
#' @param conn An EXAConnection object.
#' @param name A fully qualified table name (schema.table). Alternatively the schema can be given via the parameter 'schema' (see below).
#' @param value A data.frame containing data.
#' @param schema A schema identifier.
#' @param field_types A character vector containing the column data types, in the form of c("varchar(20)","int").
#'        If missing, the column types of the data.frame are being converted and used.
#' @param overwrite A logical indicating if existing data shall be overwritten. Default is 'FALSE',
#'        i.e. new data is appended to the DB table.
#' @param writeCols a logical or a character vector containing the cols of the DB table to be
#'      written into, in the form of c("col1","col4","col3"). If set to TRUE, then the column names of the data.frame are used.
#'      If FALSE, NA, or missing, no write columns are defined and columns are matched by column order.
#'      Default is NA. Useful to change if the DB table contains more columns than the data.frame, or if the column order differs.
#' @param ... additional parameters to be passed on to exa.writeData.
#' @return a logical indicating success.
setMethod("dbWriteTable", signature("EXAConnection","character","data.frame"),
          def = function(conn, name, value, ...) {
              EXAWriteTable(conn, name, value,...)
          })

EXAWriteTable <- function(con, tbl_name, data, schema, field_types, overwrite=FALSE, writeCols=NA, ...) {

    if(missing(schema)) { # getting the schema if missing
        n <- strsplit(tbl_name,".",fixed=TRUE)[[1]]
        if(length(n) != 2) stop("Error: Schema/table identifier ambiguous. Use SCHEMA.TABLE or provide schema separately.")
        n <- EXAupper(n)
        schema <- n[1]
        tbl_name <- n[2]
    }
    else {
        schema <- EXAupper(schema)
        tbl_name <- EXAupper(tbl_name)
    }

    dbBegin(con)
    on.exit(dbEnd(con,FALSE))

    if(dbExistsTable(con,paste0(schema,".",tbl_name))) { # if the table exists
        if(overwrite) {
            switch(as.character(sqlQuery(con, paste0("truncate table ",schema,".",tbl_name),errors=FALSE)),
                   "-1" = stop(paste("Error. Couldn't truncate table:",tbl_name,"\n",odbcGetErrMsg(con))),
                   "-2" = message(paste("Table",tbl_name,"successfully truncated.")),
                   {    print("Truncate failed.")
                       stop(odbcGetErrMsg(con))
                   }
            )
        }
    } else { # tbl does not exist, create...
        ## DDL - table definition

        # field types
        if(missing(field_types)) {
            field_types <- dbDataType(con, data)
        } else {
            if(length(field_types != ncol(data))) stop("Error creating database table: number of field
                                                       types provided does not match number of data columns in data.frame.")
        }

        # column names
        col_names <- names(data)
        if(is.null(col_names)) { ## todo
            for(i in 1:ncol(data)) {
                col_names <- append(col_names, paste0("col_",i))
            }
        }

        # create the table definition
        # first check if the schema exists, otherwise create
        tryCatch(
            {
                switch(as.character(sqlQuery(con, paste("open schema",schema),errors=FALSE)),
                       "-1" = warning(paste("Cannot open schema",schema,". Trying to create...")),
                       "-2" = message(paste("Schema",schema, "found.")),
                       {    warning(odbcGetErrMsg(con)) } )
            },
            warning=function(war) {
                switch(as.character(sqlQuery(con, paste("create schema",schema),errors=FALSE)),
                       "-1" = stop(paste("failed. Couldn't create schema:",schema)),
                       "-2" = message(paste("successful. Schema",schema,"created.")),
                       {    print("failed.")
                           stop(odbcGetErrMsg(con))
                       }
                )
            }
        )
        # setting up the table definition string
        ddl_str <- paste0("create table ",schema,".",tbl_name, "( ")
        for(i in 1:length(col_names)) {
            ddl_str <- paste0(ddl_str, col_names[i]," ", field_types[i], ", ")
        }
        ddl_str <- substr(ddl_str,1,nchar(ddl_str)-2) # remove the final comma & space
        ddl_str <- paste0(ddl_str, " )")

        switch(as.character(sqlQuery(con,ddl_str,errors=FALSE)),
               "-1" = {stop(paste0("Couldn't create table: ",schema,".",tbl_name,":\n",odbcGetErrMsg(con)))},
               "-2" = {message(paste0("Table ",schema,".",tbl_name," created:\n",ddl_str))},
               {    print("failed.")
                   stop(odbcGetErrMsg(con))
               }
        )
    } # end of else (table creation)

    if(missing(writeCols) | writeCols[1] == FALSE) {writeCols <- NA} # if write cols are missing or NA, write w/o specifying col names.
       else if (writeCols[1] == TRUE) {writeCols <- names(data)} # if TRUE, use the data.frame colnames, else use whatever is in it

    message("Writing into table...")
    if(
        exa.writeData(con, data, paste0(schema,".",tbl_name),
                  tableColumns=writeCols,...
             )
    ) {
        on.exit(dbEnd(con))
        return(TRUE)
    }
    return(FALSE)
}

#' Removes a table.
#' @family EXAConnection related objects
#' @family DML functions
#'
#' @name dbRemoveTable
#' @param conn An EXAConnection object.
#' @param name a fully qualified table identifier (schema.table) or a simple table identifier, if the
#' parameter schema is specified.
#' @param schema An optional string specifying the DB schema. Caution: use either this parameter, OR give
#'      a fully qualified identifier as giving both will result in an error.
#' @param cascade A logical indicating whether also foreign key constraints referencing the table to
#'      be deleted shall be removed. Default is FALSE.
#' @return A logicl indicating success.
setMethod("dbRemoveTable",signature("EXAConnection"),
          def = function(conn, name, schema, cascade=FALSE) {EXARemoveTable(con, name, schema, cascade)}
)



EXARemoveTable <- function(con, tbl_name, schema, cascade=FALSE) {

    if(missing(schema)) { # getting the schema if missing
        n <- strsplit(tbl_name,".",fixed=TRUE)[[1]]
        if(length(n) != 2) stop("Error: Schema/table identifier ambiguous. Use SCHEMA.TABLE or provide schema separately.")
        n <- EXAupper(n)
        schema <- n[1]
        tbl_name <- n[2]
    }
    else {
        schema <- EXAupper(schema)
        tbl_name <- EXAupper(tbl_name)
    }

    dbBegin(con)
    on.exit(dbEnd(con,FALSE))

    ddl_str <- paste0("DROP TABLE ",schema,".",tbl_name)
    if(cascade) ddl_str <- paste(ddl_str,"CASCADE CONSTRAINTS")
    switch(as.character(sqlQuery(con,ddl_str,errors=FALSE)),
           # "-1" = {stop(paste0("Couldn't remove table: ",schema,".",tbl_name,":\n",odbcGetErrMsg(con)))},
           "-2" = {
                    message(paste0("Table ",schema,".",tbl_name," removed:\n",ddl_str))
                    on.exit(dbEnd(con))
                    return(TRUE)
                  },
           {stop(paste0("Couldn't remove table: ",schema,".",tbl_name,":\n",odbcGetErrMsg(con)))
            return(FALSE)}
    )

}



#' Applies an R function to a result set.
#' The R code is transfered and deployed in an EXASOL database and executed massively parallel.
#'
#' @param res An EXAResult set.
#' @param fun A function to be applied to the result set.
#' @param simplify A logical indicating whether the result set may be transfered to the client and
#'      stored in a data.frame. Default: FALSE.
#' @return An EXAResult object relating to the result set in the EXASOL database, or if SIMPLIFY=TRUE
#'      a data.frame containing the result set.
setGeneric("dbApply",
           def = function(res,fun,...) standardGeneric("dbApply"),
           valueClass=c("DBIResult","data.frame")
)

setMethod("dbApply",signature("EXAResult"),
          def = function(res,fun,simplify=FALSE,...) {EXAApply(res,fun,simplify,...)}
)

EXAApply <- function(res, fun,simplify,...) {

# TODO

  res2
}
