#' @include EXADBI-internal.R
#' @include exa.readData.R
#' @include exa.writeData.R
#' @include exa.createScript.R
#' @include EXADBI-connection.R
#' @include EXADBI-datatype.R
#' @include EXADBI-object.R
#' @include EXADBI-driver.R
#' @include EXADBI-result.R
#' @include EXADBI-summary.R
NULL

## These are the DBI related classes and methods, which work as an abstraction layer over the basic
## exa functions and provide compabibility to the DBI package. The structure may also serve as a
## foundation for a later implemntation of a proprietary CLI interface that does not depend on RODBC.
##
## First version written in 2015 by Marcel Boldt <marcel.boldt@exasol.com>
## as part of the EXASOL R interface & SDK package. It may be used, changed and distributed freely
## with no further restrictions than already stipulated in the package license, with the exception
## that this statement must stay included and unchanged.

#' @export dbDriver

#' @export dbConnect
#' @export dbDisconnect
#' @export dbSendQuery
#' @export dbGetQuery
#' @export dbListFields
#' @export dbListTables
#' @export dbReadTable
#' @export dbWriteTable
#' @export dbExistsTable
#' @export dbRemoveTable
#' @export dbBegin
#' @export dbCommit
#' @export dbRollback

#' @export dbClearResult
#' @export dbColumnInfo
#' @export dbHasCompleted
#' @export dbGetRowsAffected
#' @export dbGetInfo
#' @export dbIsValid
#' @export fetch
#' @export dbFetch
#' @export summary
#'
#' @exportClass EXAResult
#' @exportClass EXAConnection
#' @exportClass EXADriver
#' @exportClass EXAObject
#' @exportClass EXAResult
#'
#' @export EXADataType
#'
#' @export dbEnd
#' @export dbCurrentSchema
#' @export exasol_driver
#' @export exa
#'
#' @export exa.readData
#' @export exa.writeData
#' @export exa.createScript
#'
NULL
