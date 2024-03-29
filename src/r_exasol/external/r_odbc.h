#ifndef R_EXASOL_R_ODBC_H
#define R_EXASOL_R_ODBC_H

#include <r_exasol/external/sql.h>
#include <r_exasol/external/r.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//THIS IS JUST COPIED FROM https://github.com/cran/RODBCext/blob/master/src/RODBC.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct rodbcHandle {
    SQLHDBC	    hDbc;      /* connection handle */
    SQLHSTMT	  hStmt;     /* statement handle */
    SQLLEN	    nRows;     /* number of rows and columns in result set */
    SQLSMALLINT	nColumns;
    int channel;      /* as stored on the R-level object */
    int id;           /* ditto */
    int useNRows;     /* value of believeNRows */
    /* entries used to bind data for result sets and updates */
    void	*ColData;	  /* this will be allocated as an array */
    int		nAllocated; /* how many cols were allocated */
    SQLUINTEGER	rowsFetched;	/* use to indicate the number of rows fetched */
    SQLUINTEGER	rowArraySize;	/* use to indicate the number of rows we expect back */
    SQLUINTEGER	rowsUsed;	/* for when we fetch more than we need */
    void	*msglist;	/* root of linked list of messages */
    SEXP  extPtr;		/* address of external pointer for this
                       channel, so we can clear it */
} RODBCHandle, *pRODBCHandle;

#endif //R_EXASOL_R_ODBC_H
