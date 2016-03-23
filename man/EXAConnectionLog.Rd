% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/EXADBI.R
\name{EXAConnectionLog}
\alias{EXAConnectionLog}
\title{Shows the EXASOL ODBC connection log.}
\usage{
EXAConnectionLog(con, view = TRUE)
}
\arguments{
\item{con}{An EXAConnection object}

\item{view}{logical If true, the log is shown in R View, otherwise, a data.frame is returned.}
}
\value{
a data.frame, if View is FALSE.
}
\description{
Shows the EXASOL ODBC connection log.
}
