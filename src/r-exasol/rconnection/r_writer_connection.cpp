#include <r-exasol/rconnection/r_writer_connection.h>

#include <cstdarg>
#include <cstdio>

#define class clss //need to hide class as it is reserved in C++, but used as variable name in Connections.h
#define private priv //need to hide private as it is reserved in C++, but used as variable name in Connections.h
#include <R_ext/Connections.h>
#include <cstdlib>
#include <utility>

#undef class
#undef private

#include "connection_hook.h"

namespace rcon = exa::rconnection;

extern "C" {


extern void Rf_set_iconv(::Rconnection con);
extern int dummy_vfprintf(::Rconnection con, const char *format, va_list ap);


static int pipe_vfprintf(const ::Rconnection con, const char *format, va_list ap) {
    return dummy_vfprintf(con, format, ap);
}

static size_t pipe_write(const void *ptr, size_t size, size_t nitems,
                         const ::Rconnection con) {
    size_t  retVal = 0;
    std::weak_ptr<exa::writer::Writer>* writer =
            rcon::getConnectionHook<exa::writer::Writer>(con);
    if(writer) {
        auto writerLocked = writer->lock();
        if (writerLocked) {
            retVal = writerLocked->pipe_write(ptr, size, nitems);
        }
    }
    return retVal;
}

static int pipe_fflush(::Rconnection con) {
    int retVal = 0;
    std::weak_ptr<exa::writer::Writer>* writer =
            rcon::getConnectionHook<exa::writer::Writer>(con);
    if(writer) {
        auto writerLocked = writer->lock();
        if (writerLocked) {
            retVal = writerLocked->pipe_fflush();
        }
    }
    return retVal;
}

}

rcon::RWriterConnection::RWriterConnection(std::weak_ptr<writer::Writer> writer)
: mWriter(std::move(writer))
, mConn(nullptr) {}

SEXP rcon::RWriterConnection::create() {
    SEXP r_custom_connection;
    PROTECT(r_custom_connection = ::R_new_custom_connection("exasol", "w", "textConnection", &mConn));

    mConn->isopen = TRUE;
    mConn->blocking = TRUE;
    mConn->canread = FALSE;
    mConn->canwrite = TRUE;
    mConn->vfprintf = &pipe_vfprintf;
    mConn->write = &pipe_write;
    mConn->fflush = &pipe_fflush;
    mConn->save = -1000;
    //Reserve memory on the heap for storing the connection hook.
    //R_ext will delete this memory later (see https://github.com/wch/r-source/blob/68251d4dd24b6bd970e5a6a92d5d07a3cf8a383d/src/main/connections.c#L405)
    mConn->priv = allocConnectionHook<exa::writer::Writer>();
    storeConnectionHook(mConn, &mWriter);
    Rf_set_iconv(mConn);
    UNPROTECT(1);
    return r_custom_connection;
}

void exa::rconnection::RWriterConnection::release() {
    storeConnectionHook<exa::writer::Writer>(mConn, nullptr);
}


