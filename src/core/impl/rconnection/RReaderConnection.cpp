//
// Created by thomas on 08/07/2021.
//

#include "RReaderConnection.h"
#include <impl/transfer/reader/Reader.h>
#include <cstdlib>


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
    exa::reader::Reader * reader = *((exa::reader::Reader **) con->priv);
    return reader->pipe_read(ptr, size, nitems);
}

static int file_fgetc_internal(const Rconnection con) {
    exa::reader::Reader * reader = *((exa::reader::Reader **) con->priv);
    return reader->fgetc();
}

}

namespace rcon = exa::rconnection;

rcon::RReaderConnection::RReaderConnection(reader::Reader * reader)
: mReader(reader)
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
    mConn->priv = (void*)::malloc(sizeof(reader::Reader*));
    *(static_cast<reader::Reader**>(mConn->priv)) = mReader;
    Rf_set_iconv(mConn);
    UNPROTECT(1);
    return r_custom_connection;
}