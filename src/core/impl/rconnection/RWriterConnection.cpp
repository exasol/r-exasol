//
// Created by thomas on 08/07/2021.
//

#include <impl/rconnection/RWriterConnection.h>

#include <cstdarg>
#include <cstdio>

#define class clss //need to hide class as it is reserved in C++, but used as variable name in Connections.h
#define private priv //need to hide private as it is reserved in C++, but used as variable name in Connections.h
#include <R_ext/Connections.h>
#include <cstdlib>

#undef class
#undef private

extern "C" {


extern void Rf_set_iconv(Rconnection con);
extern int dummy_vfprintf(Rconnection con, const char *format, va_list ap);


static int pipe_vfprintf(const Rconnection con, const char *format, va_list ap) {
    return dummy_vfprintf(con, format, ap);
}

static size_t pipe_write(const void *ptr, size_t size, size_t nitems,
                         const Rconnection con) {
    exa::writer::Writer * writer = *((exa::writer::Writer **) con->priv);
    return writer->pipe_write(ptr, size, nitems);
}

static int pipe_fflush(Rconnection con) {
    exa::writer::Writer * writer = *((exa::writer::Writer **) con->priv);
    return writer->pipe_fflush();
}

}

namespace rcon = exa::rconnection;

rcon::RWriterConnection::RWriterConnection(exa::writer::Writer * writer)
: mWriter(writer)
, mConn(nullptr) {}

SEXP rcon::RWriterConnection::create() {
    SEXP r_custom_connection;
    PROTECT(r_custom_connection = R_new_custom_connection("exasol", "w", "textConnection", &mConn));

    mConn->isopen = TRUE;
    mConn->blocking = TRUE;
    mConn->canread = FALSE;
    mConn->canwrite = TRUE;
    mConn->vfprintf = &pipe_vfprintf;
    mConn->write = &pipe_write;
    mConn->fflush = &pipe_fflush;
    mConn->save = -1000;
    mConn->priv = (void*)::malloc(sizeof(RWriterConnection*));
    *(static_cast<writer::Writer**>(mConn->priv)) = mWriter;
    Rf_set_iconv(mConn);
    UNPROTECT(1);
    return r_custom_connection;
}