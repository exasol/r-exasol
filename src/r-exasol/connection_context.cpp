#include <r-exasol/connection_context.h>


#include <r-exasol/connection/connection_factory_impl.h>
#include <r-exasol/rconnection/r_connection.h>
#include <r-exasol/connection/connection_controller.h>

#include <r-exasol/rconnection/r_reader_connection.h>
#include <r-exasol/rconnection/r_writer_connection.h>
#include <r-exasol/odbc/odbc_session_info_impl.h>

namespace exa {
    void onError(std::string e) {
        ::error(e.c_str());
    }
}

int exa::ConnectionContext::initConnection(const char *host, int port) {
    destroyConnection(1);
    mConnectionController = std::make_unique<exa::ConnectionController>(mConnectionFactory, exa::onError);
    return mConnectionController->connect(host, static_cast<uint16_t>(port)) ? 0 : -1;
}

SEXP exa::ConnectionContext::copyHostName() {
    SEXP host = nullptr;
    if (mConnectionController) {
        PROTECT(host = allocVector(STRSXP, 1));
        const std::string hostName = mConnectionController->getHostInfo().first;
        SET_STRING_ELT(host, 0, mkChar(hostName.c_str()));
        UNPROTECT(1);
    }
    return host;
}

SEXP exa::ConnectionContext::copyHostPort() {
    uint16_t hostPort = -1;
    if (mConnectionController) {
        hostPort = mConnectionController->getHostInfo().second;
    }
    return ScalarInteger(hostPort);
}

int exa::ConnectionContext::destroyConnection(int closeFd) {
    bool wasDone(false);

    if (mConnection) {
        mConnection->release();
    }
    if (mConnectionController) {
        wasDone = mConnectionController->shutDown();
    }
    if(!closeFd && !wasDone) {
        ::warning("Transfer was not done jet.");
    }
    mConnection.reset();
    mConnectionController.reset();
    return wasDone ? 0 : -1;
}

SEXP exa::ConnectionContext::createReadConnection(::pRODBCHandle handle, ::SQLCHAR *query) {
    SEXP retVal = nullptr;
    if (mConnectionController) {
        std::weak_ptr<exa::reader::Reader> reader = mConnectionController->startReading(exa::OdbcSessionInfoImpl(handle, query),
                                                                            exa::ProtocolType::http);
        if (!reader.expired()) {
            auto readerConnection = std::make_unique<exa::rconnection::RReaderConnection>(reader);
            retVal = readerConnection->create();
            mConnection = std::move(readerConnection);
        }
    }
    return retVal;
}

SEXP exa::ConnectionContext::createWriteConnection(::pRODBCHandle handle, ::SQLCHAR *query) {
    SEXP retVal = nullptr;
    if (mConnectionController) {
        std::weak_ptr<exa::writer::Writer> writer = mConnectionController->startWriting(exa::OdbcSessionInfoImpl(handle, query),
                                                                              exa::ProtocolType::http);
        if (!writer.expired()) {
            auto writeConnection = std::make_unique<exa::rconnection::RWriterConnection>(writer);
            retVal = writeConnection->create();
            mConnection = std::move(writeConnection);
        }
    }
    return retVal;
}
