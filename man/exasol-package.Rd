\name{exasol-package}
\alias{exasol-package}
\alias{exasol}
\docType{package}
\title{EXASolution R Package}
\description{
The EXASolution R Package offers functionality to interact with the EXASolution database out of R programs. It is developed as a wrapper around ORDBC and extends ORDBC in two main aspects:

\enumerate{
\item It offers fast data transfer between EXASolution and R, multiple times faster than RODBC. This is achieved by using a proprietary transfer channel which is optimized for batch loading.
Please read the R help of exa.readData() and exa.writeData() for details.

\item It makes it convenient to run parts of your R code in parallel on the EXASolution database, using EXASolution R UDF scripts behind the scenes. For example you can define an R function and execute it in parallel on different groups of data in an EXASolution table.
Please read the R help of exa.createScript() function for details.
}

The help is available directly in R via:

\itemize{
\item help(exa.readData)
\item help(exa.writeData)
\item help(exa.createScript)
}
}
\details{
\tabular{ll}{
Package: \tab exasol\cr
Type: \tab Package\cr
Version: \tab 5.0.7\cr
Date: \tab 2015-03-09\cr
License: \tab BSD_3_clause + file LICENSE\cr
}
}
\author{EXASOL AG <support@exasol.com>}
\keyword{ package }
