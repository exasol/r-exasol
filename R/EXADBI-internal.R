
## These are sub-methods used by the DBI methods defined in EXADBI.R.
##
## First version written in 2015 by Marcel Boldt <marcel.boldt@exasol.com>
## as part of the EXASOL R interface & SDK package. It may be used, changed and distributed freely
## with no further restrictions than already stipulated in the package license, with the exception
## that this statement must stay included and unchanged.



# Tries to extrapolate the schema name from the statement given. It looks for strings framed by 'from' and a dot (not case sensitive).
#
# @param stmt The statement used
# @return a character vector containing all schemas found, or alt
grep_schema <- function(stmt , statement = TRUE) {
  g1 <-   gregexpr("from(([\\s.]\"[^\"]+\")|([\\s.]\\w+))+?([;\\s)]|$)",  # grep all from stmts.
                   stmt, perl = TRUE, ignore.case = TRUE)
  ids <- gsub("from ","",unlist(regmatches(stmt, g1)), ignore.case = TRUE) # remove the "from "s

  #   ids <- sapply(ids, function(x)     # remove the final chr if present
  #                   if(substring(x, nchar(x)) %in% c(";"," ",")"))
  #                     {
  #                       return(substr(x,1,nchar(x)-1))
  #                     } else return(x)
  #           , USE.NAMES = FALSE )

  g2 <-   gregexpr("^(\\w+?\\.)|^(\"[^\"]+?\"\\.)", # grep all schema id
                   ids, perl = TRUE, ignore.case = TRUE)
  schemas <- unlist(regmatches(ids, g2))
  schemas <- substr(schemas,1,nchar(schemas)-1)
  schemas
}

# Changes an identifier into uppercase, except for it is quoted.
#
# name EXAupper
# param identifier A character vector containing one or many (table, schema, column,...) identifiers.
# return A character vector containing one or many processed identifiers.
# export
EXAupper <- function(identifier) {
  for (i in 1:length(identifier)) {
    if (substr(identifier[i], 1, 1) == "\"" &
        substr(identifier[i], nchar(identifier[i]), nchar(identifier[i])) == "\"") {
      identifier[i] <- identifier[i] # quoted
    } else {
      identifier[i] <- toupper(identifier[i]) # not quoted
    }
  }
  identifier
}

# Takes an identifier, and transforms it into an EXASOL compatible identifier. In Detail: it
#  - removes surrounding whitespaces
#  - doubles quotes inside the string,
#  - changes dots to underscores,
#  - adds surrounding quotes.
#  param id A character vector containing identifiers.
#  param quotes The kind of quotes to be used: double-qoutes for identifiers with special chars, single
#  quotes for strings.
#  return A list of character vectors, each containing schema and table identifier. conversion
#  via as.data.frame fails for the method strips escaped quotes.
processIDs <- function(id, quotes = "\"") {

  if (length(id) < 1 ) return("")
#   if (length(id) == 1 )
#     if (gsub("^\\s+|\\s+$", "", id) == "" | is.null(id) | is.na(id))
#       return(sql("")) # empty string is NULL in EXASOL
#

  if (length(id) == 1 & is.na(id) ) return(NA)

  id <- gsub("^\\s+|\\s+$", "", id) # remove surr whitespaces
  # remove surr quotes if present
  id <-
    sapply(id, function(x) {
      if (is.na(x) | x == "") return("NA")
      if (substr(x, 1,1) == "\"" & substring(x, nchar(x)) == "\"")
        return(substr(x, 2, nchar(x) - 1))
      x
    })

  id <-
    gsub('"', '""', id, fixed = TRUE) #double every remaining quote
  id <- gsub('.', '_', id, fixed = TRUE)# change dots to underscores

  #  ...add surr quotes
  paste0(quotes,make.unique(encodeString(id)),quotes)
}

# This method recognises schema and table identifiers, and applies proper quoting.
# @param string The input string containing identifiers.
# @param statement A boolean indicating whether the input string is a whole statement, or a fully qualified identifier.
# Default: An identifier
# @param quoting_style A string, either "R" or "EXASOL", indicating the quoting behaviour. If "R", the R identifier
# behaviour is kept, which means all identifiers are quoted when sent to EXASOL. if "EXASOL", identifiers
# are being converted to uppercase, except for they are quoted.
# @param quotes The quotes to be used. (see method `processIDs()`)
# @return A character vector containing schema & table identifiers.
EXAGetIdentifier <-
  function (string, statement = FALSE, quoting_style = "R", quotes = "\"") {

    if (statement) {
      stmt <- string

      g1 <-
        gregexpr(
          "from(([\\s.]\"[^\"]+\")|([\\s.]\\w+))+?([;\\s)]|$)",  # grep all from stmts.
          stmt, perl = TRUE, ignore.case = TRUE
        )
      ids <-
        gsub("from ","",unlist(regmatches(stmt, g1)), ignore.case = TRUE) # remove the "from "s

      ids <-
        sapply(ids, function(x)
          # remove the final chr if present
          if (substring(x, nchar(x)) %in% c(";"," ",")"))
          {
            return(substr(x,1,nchar(x) - 1))
          } else
            return(x)
          , USE.NAMES = FALSE)
    } else {
      ids <- string
    }

    ds <- lapply(ids, function(id) {
      g2 <-
        gregexpr("^(\\w+?\\.)|^(\"[^\"]+?\"\\.)", # grep all schema id
                 id, perl = TRUE, ignore.case = TRUE)
      schemas <- unlist(regmatches(id, g2))
      schemas <-
        substr(schemas,1,nchar(schemas) - 1) # remove the final dot

      if (quoting_style == "R") {
        # quoting according to style chosen
        schemas <- processIDs(schemas, quotes = quotes)
      } else {
        schemas <- EXAupper(schemas)
      }

      g3 <-
        gregexpr("(\\w+?)$|(\"[^\"]+?\")$", # grep all table id
                 id, perl = TRUE, ignore.case = TRUE)
      tables <- unlist(regmatches(id, g3))

      if (quoting_style == "R") {
        # quoting according to style chosen
        tables <- processIDs(tables, quotes = quotes)
      } else {
        tables <- EXAupper(tables)
      }

      c(ifelse(length(schemas) == 0, "", schemas), tables)
    })
    ds

    # as.data.frame(ds, stringsAsFactors = FALSE, optional = TRUE) # doesn't work as it strips quotes
  }



