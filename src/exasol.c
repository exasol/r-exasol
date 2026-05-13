#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

/* Testthat C++ test runner (defined in tests/test_runner.cpp via testthat.h) */
extern SEXP run_testthat_tests(SEXP);

/* Rcpp-generated wrappers (defined in RcppExports.cpp) */
extern SEXP _exasol_exaWsConnect(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP _exasol_exaWsExecute(SEXP, SEXP);
extern SEXP _exasol_exaWsFetch(SEXP, SEXP, SEXP, SEXP);
extern SEXP _exasol_exaWsCloseResultSet(SEXP, SEXP);
extern SEXP _exasol_exaWsDisconnect(SEXP);
extern SEXP _exasol_exaWsSetAttributes(SEXP, SEXP);
extern SEXP _exasol_exaWsGetAttributes(SEXP);
extern SEXP _exasol_exaWsIsConnected(SEXP);
extern SEXP _exasol_asyncRODBCIOStart(SEXP, SEXP, SEXP);
extern SEXP _exasol_asyncRODBCProxyHost(void);
extern SEXP _exasol_asyncRODBCProxyPort(void);
extern SEXP _exasol_asyncRODBCQueryStart(SEXP, SEXP, SEXP, SEXP);
extern SEXP _exasol_asyncRODBCQueryFinish(SEXP);
extern SEXP _exasol_asyncEnableTracing(SEXP);

static R_CallMethodDef CallEntries[] = {
    {"run_testthat_tests",            (DL_FUNC) &run_testthat_tests,            1},
    {"_exasol_exaWsConnect",          (DL_FUNC) &_exasol_exaWsConnect,          7},
    {"_exasol_exaWsExecute",          (DL_FUNC) &_exasol_exaWsExecute,          2},
    {"_exasol_exaWsFetch",            (DL_FUNC) &_exasol_exaWsFetch,            4},
    {"_exasol_exaWsCloseResultSet",   (DL_FUNC) &_exasol_exaWsCloseResultSet,   2},
    {"_exasol_exaWsDisconnect",       (DL_FUNC) &_exasol_exaWsDisconnect,       1},
    {"_exasol_exaWsSetAttributes",    (DL_FUNC) &_exasol_exaWsSetAttributes,    2},
    {"_exasol_exaWsGetAttributes",    (DL_FUNC) &_exasol_exaWsGetAttributes,    1},
    {"_exasol_exaWsIsConnected",      (DL_FUNC) &_exasol_exaWsIsConnected,      1},
    {"_exasol_asyncRODBCIOStart",     (DL_FUNC) &_exasol_asyncRODBCIOStart,     3},
    {"_exasol_asyncRODBCProxyHost",   (DL_FUNC) &_exasol_asyncRODBCProxyHost,   0},
    {"_exasol_asyncRODBCProxyPort",   (DL_FUNC) &_exasol_asyncRODBCProxyPort,   0},
    {"_exasol_asyncRODBCQueryStart",  (DL_FUNC) &_exasol_asyncRODBCQueryStart,  4},
    {"_exasol_asyncRODBCQueryFinish", (DL_FUNC) &_exasol_asyncRODBCQueryFinish, 1},
    {"_exasol_asyncEnableTracing",    (DL_FUNC) &_exasol_asyncEnableTracing,    1},
    {NULL, NULL, 0}
};

void R_init_exasol(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    R_forceSymbols(dll, TRUE);
}
