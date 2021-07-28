//
// Created by thomas on 08/07/2021.
//

#include <r-exasol/rconnection/RReaderConnection.h>
#include <r-exasol/connection/Reader.h>
#include <cstdlib>
#include <utility>


extern "C" {

#define class clss //need to hide class as it is reserved in C++, but used as variable name in Connections.h
#define private priv //need to hide private as it is reserved in C++, but used as variable name in Connections.h
#include <R_ext/Connections.h>
#undef class
#undef private

extern int dummy_fgetc(Rconnection con);
extern void Rf_set_iconv(Rconnection con);


static size_t pipe_read(void *ptr, const size_t size, const size_t nitems,
                        const Rconnection con) {
    size_t retVal = 0;
    std::weak_ptr<exa::reader::Reader>* reader =
            *((std::weak_ptr<exa::reader::Reader> **) con->priv);

    if(reader) {
        auto readerLocked = reader->lock();
        if (readerLocked) {
            retVal = readerLocked->pipe_read(ptr, size, nitems);
        }
    }
    return retVal;
}

static int file_fgetc_internal(const Rconnection con) {
    int retVal = 0;
    std::weak_ptr<exa::reader::Reader>* reader =
            *((std::weak_ptr<exa::reader::Reader> **) con->priv);
    if(reader) {
        auto readerLocked = reader->lock();
        if (readerLocked) {
            retVal = readerLocked->fgetc();
        }
    }
    return  retVal;
}

}

namespace rcon = exa::rconnection;

rcon::RReaderConnection::RReaderConnection(std::weak_ptr<reader::Reader> reader)
: mReader(std::move(reader))
, mConn(nullptr){}

SEXP rcon::RReaderConnection::create() {
    SEXP r_custom_connection;
    PROTECT(r_custom_connection = R_new_custom_connection("exasol", "r", "textConnection", &mConn));

    mConn->isopen = TRUE;
    mConn->blocking = TRUE;
    mConn->canread = TRUE;
    mConn->canwrite = FALSE;
    mConn->read = &pipe_read;
    mConn->fgetc = &dummy_fgetc;
    mConn->fgetc_internal = &file_fgetc_internal;
    mConn->save = -1000;
    //mConn->priv allows us to store a private pointer to anything.
    //However, RExt Connections will free this memory if it is not null when cleaning up connection
    //As we want to keep control about when to delete Reader, we allocate memory for one pointer;
    //and store the pointer to the pointer of the Reader here
    mConn->priv = (void*)::malloc(sizeof(std::weak_ptr<exa::reader::Reader>*));
    *(static_cast<std::weak_ptr<exa::reader::Reader>**>(mConn->priv)) = &mReader;
    Rf_set_iconv(mConn);
    UNPROTECT(1);
    return r_custom_connection;
}

void exa::rconnection::RReaderConnection::release() {
    *(static_cast<std::weak_ptr<exa::reader::Reader>**>(mConn->priv)) = nullptr;
}
