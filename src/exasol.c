#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef ERROR
#undef ERROR
#endif
#include <Rdefines.h>

#include <stdio.h>
#include <connection.h>


extern SEXP run_testthat_tests(SEXP);


SEXP asyncRODBCIOStart(SEXP hostA, SEXP portA, SEXP protocolA) {
    const int port = asInteger(portA);
    const char *host = CHAR(STRING_ELT(hostA, 0));
    const char *protocol = CHAR(STRING_ELT(protocolA, 0));
    int retVal = initConnection(host, port, protocol);
    return ScalarInteger(retVal);
}

SEXP asyncRODBCProxyHost() {
    return copyHostName();
}

SEXP asyncRODBCProxyPort() {
    return copyHostPort();
}

SEXP asyncRODBCQueryStart(SEXP chan, SEXP query, SEXP protocol, SEXP writerA) {
    SEXP retVal = ScalarInteger(-1);
    const int writer = asInteger(writerA);
    pRODBCHandle rodbc = R_ExternalPtrAddr(chan);
    SQLCHAR *q = (SQLCHAR *) translateChar(STRING_ELT(query, 0));
    const char *protocol_native = (const char *) translateChar(STRING_ELT(protocol, 0));
    if (rodbc != NULL && q != NULL) {
        retVal = writer ? createWriteConnection(rodbc, q, protocol_native) : createReadConnection(rodbc, q, protocol_native);
    } else {
        error("Could not get RODBC structure from channel");
    }
    return retVal;
}

SEXP asyncRODBCQueryFinish(SEXP checkWasDone) {
    const int checkWasDoneNative = asInteger(checkWasDone);
    const int retVal = destroyConnection(checkWasDoneNative);
    return ScalarInteger(retVal);
}

SEXP asyncEnableTracing(SEXP tracefile) {
    const char *tracefileNative = CHAR(STRING_ELT(tracefile, 0));
    return ScalarInteger(enableTracing(tracefileNative));
}

#include <R_ext/Rdynload.h>

R_CallMethodDef CallEntries[] = {
    {"asyncRODBCIOStart", (DL_FUNC) &asyncRODBCIOStart, 3},
    {"asyncRODBCProxyHost", (DL_FUNC) &asyncRODBCProxyHost, 0},
    {"asyncRODBCQueryStart", (DL_FUNC) &asyncRODBCQueryStart, 4},
    {"asyncRODBCProxyPort", (DL_FUNC) &asyncRODBCProxyPort, 0},
    {"asyncRODBCQueryFinish", (DL_FUNC) &asyncRODBCQueryFinish, 1},
    {"asyncEnableTracing", (DL_FUNC) &asyncEnableTracing, 1},
    {"run_testthat_tests", (DL_FUNC) &run_testthat_tests, 1},
    {NULL, NULL, 0}
};

void R_init_exasol(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    R_forceSymbols(dll, TRUE);
}
