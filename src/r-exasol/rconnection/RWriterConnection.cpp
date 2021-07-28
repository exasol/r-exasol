//
// Created by thomas on 08/07/2021.
//

#include <r-exasol/rconnection/RWriterConnection.h>

#include <cstdarg>
#include <cstdio>

#define class clss //need to hide class as it is reserved in C++, but used as variable name in Connections.h
#define private priv //need to hide private as it is reserved in C++, but used as variable name in Connections.h
#include <R_ext/Connections.h>
#include <cstdlib>
#include <utility>

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
    size_t  retVal = 0;
    std::weak_ptr<exa::writer::Writer>* writer =
            *((std::weak_ptr<exa::writer::Writer> **) con->priv);
    if(writer) {
        auto writerLocked = writer->lock();
        if (writerLocked) {
            retVal = writerLocked->pipe_write(ptr, size, nitems);
        }
    }
    return retVal;
}

static int pipe_fflush(Rconnection con) {
    int retVal = 0;
    std::weak_ptr<exa::writer::Writer>* writer =
            *((std::weak_ptr<exa::writer::Writer> **) con->priv);
    if(writer) {
        auto writerLocked = writer->lock();
        if (writerLocked) {
            retVal = writerLocked->pipe_fflush();
        }
    }
    return retVal;
}

}

namespace rcon = exa::rconnection;

rcon::RWriterConnection::RWriterConnection(std::weak_ptr<writer::Writer> writer)
: mWriter(std::move(writer))
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
    //mConn->priv allows us to store a private pointer to anything.
    //However, RExt Connections will free this memory if it is not null when cleaning up connection
    //As we want to keep control about when to delete Writer, we allocate memory for one pointer;
    //and store the pointer to the pointer of the Writer here
    mConn->priv = (void*)::malloc(sizeof(std::weak_ptr<exa::writer::Writer>*));
    *(static_cast<std::weak_ptr<exa::writer::Writer>**>(mConn->priv)) = &mWriter;
    Rf_set_iconv(mConn);
    UNPROTECT(1);
    return r_custom_connection;
}

void exa::rconnection::RWriterConnection::release() {
    *(static_cast<std::weak_ptr<exa::writer::Writer>**>(mConn->priv)) = nullptr;
}

