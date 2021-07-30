#include <r-exasol/rconnection/RReaderConnection.h>
#include <r-exasol/connection/Reader.h>
#include <utility>

#define class clss //need to hide class as it is reserved in C++, but used as variable name in Connections.h
#define private priv //need to hide private as it is reserved in C++, but used as variable name in Connections.h
#include <R_ext/Connections.h>
#undef class
#undef private

#include "ConnectionHook.h"

namespace rcon = exa::rconnection;

extern "C" {

extern int dummy_fgetc(Rconnection con);
extern void Rf_set_iconv(Rconnection con);


static size_t pipe_read(void *ptr, const size_t size, const size_t nitems,
                        const ::Rconnection con) {
    size_t retVal = 0;
    std::weak_ptr<exa::reader::Reader>* reader =
            rcon::getConnectionHook<exa::reader::Reader>(con);

    if(reader) {
        auto readerLocked = reader->lock();
        if (readerLocked) {
            retVal = readerLocked->pipe_read(ptr, size, nitems);
        }
    }
    return retVal;
}

static int file_fgetc_internal(const ::Rconnection con) {
    int retVal = 0;
    std::weak_ptr<exa::reader::Reader>* reader =
            rcon::getConnectionHook<exa::reader::Reader>(con);
    if(reader) {
        auto readerLocked = reader->lock();
        if (readerLocked) {
            retVal = readerLocked->fgetc();
        }
    }
    return  retVal;
}

}

rcon::RReaderConnection::RReaderConnection(std::weak_ptr<reader::Reader> reader)
: mReader(std::move(reader))
, mConn(nullptr){}

SEXP rcon::RReaderConnection::create() {
    SEXP r_custom_connection;
    PROTECT(r_custom_connection = ::R_new_custom_connection("exasol", "r", "textConnection", &mConn));

    mConn->isopen = TRUE;
    mConn->blocking = TRUE;
    mConn->canread = TRUE;
    mConn->canwrite = FALSE;
    mConn->read = &pipe_read;
    mConn->fgetc = &dummy_fgetc;
    mConn->fgetc_internal = &file_fgetc_internal;
    //Reserve memory on the heap for storing the connection hook.
    //R_ext will delete this memory later (see https://github.com/wch/r-source/blob/68251d4dd24b6bd970e5a6a92d5d07a3cf8a383d/src/main/connections.c#L405)
    mConn->priv = allocConnectionHook<exa::reader::Reader>();
    storeConnectionHook(mConn, &mReader);
    mConn->save = -1000;
    Rf_set_iconv(mConn);
    UNPROTECT(1);
    return r_custom_connection;
}

void exa::rconnection::RReaderConnection::release() {
    storeConnectionHook<exa::reader::Reader>(mConn, nullptr);
}
