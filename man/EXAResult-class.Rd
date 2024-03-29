% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/EXADBI-result.R
\docType{class}
\name{EXAResult-class}
\alias{EXAResult-class}
\alias{EXAResult}
\title{An object that is associated with a result set in an EXASOL Database.}
\description{
The result set is persisted
in a DB table, which is dropped when the object is deleted in R (on garbage collection), or manually by
`dbClearResult()'. R versions before 3.3.0 do not finalise objects on R exit, so if R is quit with an
active EXAResult object, the table may stay in the DB.
}
\section{Fields}{

\describe{
\item{\code{connection}}{An EXAConnection object.}

\item{\code{statement}}{A string containing the SQL query statement.}

\item{\code{rows_fetched}}{An int reflecting the rows already fetched from the DB.}

\item{\code{rows_affected}}{An int reflecting the length of the dataset in the DB.}

\item{\code{is_complete}}{A logical indicating if the result set has been entirely fetched.}

\item{\code{with_output}}{A logical indicating whether the query produced a result set.}

\item{\code{profile}}{A data.frame containing profile information on the query.}

\item{\code{columns}}{A data.frame containing column metadata.}

\item{\code{temp_result_tbl}}{A string reflecting the name of the (temporary) table that holds the result set.}

\item{\code{query_sent_time}}{A POSIXct timestamp indicating when the query was sent to the DB.}

\item{\code{errors}}{A character vector containing errors.}

\item{\code{default_fetch_rec}}{An int reflecting the default fetch size.}
}}

\section{Methods}{

\describe{
\item{\code{addRowsFetched(x)}}{Add an int (the length of a newly fetched result set) to rows_fetched.}

\item{\code{refreshMetaData(x)}}{Refreshes the object's metadata.}
}}

\seealso{
\code{\link{DBIResult-class}}

Other DBI classes: 
\code{\link{EXAConnection-class}},
\code{\link{EXADriver-class}},
\code{\link{EXAObject-class}}

Other EXAResult related objects: 
\code{\link{dbClearResult,EXAResult-method}},
\code{\link{dbListFields,EXAConnection,character-method}},
\code{\link{fetch,EXAResult,numeric-method}}
}
\author{
EXASOL AG <opensource@exasol.com>
}
\concept{DBI classes}
\concept{EXAResult related objects}
