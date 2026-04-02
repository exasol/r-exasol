#include <r_exasol/rconnection/r_writer_connection.h>
#include <r_exasol/external/r_conn.h>
#include <r_exasol/rconnection/connection_hook.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <utility>

namespace {
    constexpr int kConnectionSaveDefault = -1000;
}


namespace rcon = exa::rconnection;

extern "C" {


extern void Rf_set_iconv(::Rconnection con);
extern int dummy_vfprintf(::Rconnection con, const char *format, va_list argptr);


static int pipe_vfprintf(const ::Rconnection con, const char *format, va_list argptr) {
    return dummy_vfprintf(con, format, argptr);
}

static size_t pipe_write(const void *ptr, size_t size, size_t nitems,
                         const ::Rconnection con) {
    size_t  retVal = 0;
    std::weak_ptr<exa::writer::Writer>* writer =
            rcon::getConnectionHook<exa::writer::Writer>(con);
    if(writer != nullptr) {
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
    if(writer != nullptr) {
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
    mConn->save = kConnectionSaveDefault;
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


