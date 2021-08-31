#include <r_exasol/connection_context.h>


#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/rconnection/r_connection.h>
#include <r_exasol/connection/connection_controller.h>

#include <r_exasol/rconnection/r_reader_connection.h>
#include <r_exasol/rconnection/r_writer_connection.h>
#include <r_exasol/odbc/odbc_session_info_impl.h>

namespace exa {
    void onError(std::string e) {
        ::error(e.c_str());
    }
}


#define CON_CONTEXT_STACK_PRINTER STACK_PRINTER( exa::ConnectionContext);


int exa::ConnectionContext::initConnection(const char *host, int port, const char* protocol) {
    CON_CONTEXT_STACK_PRINTER;
    destroyConnection(false);
    mConnectionController = std::make_unique<exa::ConnectionController>(mConnectionFactory, exa::onError);
    return mConnectionController->connect(convertProtocol(protocol), host, static_cast<uint16_t>(port)) ? 0 : -1;
}

SEXP exa::ConnectionContext::copyHostName() {
    CON_CONTEXT_STACK_PRINTER;
    SEXP host = nullptr;
    if (mConnectionController) {
        PROTECT(host = allocVector(STRSXP, 1));
        const std::string hostName = mConnectionController->getProxyHost();
        SET_STRING_ELT(host, 0, mkChar(hostName.c_str()));
        UNPROTECT(1);
    }
    return host;
}

SEXP exa::ConnectionContext::copyHostPort() {
    CON_CONTEXT_STACK_PRINTER;
    uint16_t hostPort = -1;
    if (mConnectionController) {
        hostPort = mConnectionController->getProxyPort();
    }
    return ScalarInteger(hostPort);
}

int exa::ConnectionContext::destroyConnection(bool checkDone) {
    CON_CONTEXT_STACK_PRINTER;
    bool wasDone(false);

    if (mConnection) {
        mConnection->release();
    }
    if (mConnectionController) {
        wasDone = mConnectionController->shutDown();
    }
    if(checkDone && !wasDone) {
        ::warning("Transfer was not done jet.");
    }
    mConnection.reset();
    mConnectionController.reset();
    return wasDone ? 0 : -1;
}

SEXP exa::ConnectionContext::createReadConnection(::pRODBCHandle handle, ::SQLCHAR *query, const char* protocol) {
    CON_CONTEXT_STACK_PRINTER;
    SEXP retVal = nullptr;
    if (mConnectionController) {
        std::weak_ptr<exa::reader::Reader> reader =
                mConnectionController->startReading(exa::OdbcSessionInfoImpl(handle, query));
        if (!reader.expired()) {
            auto readerConnection = std::make_unique<exa::rconnection::RReaderConnection>(reader);
            retVal = readerConnection->create();
            mConnection = std::move(readerConnection);
        }
    }
    return retVal;
}

SEXP exa::ConnectionContext::createWriteConnection(::pRODBCHandle handle, ::SQLCHAR *query, const char* protocol) {
    CON_CONTEXT_STACK_PRINTER;
    SEXP retVal = nullptr;
    if (mConnectionController) {
        std::weak_ptr<exa::writer::Writer> writer =
                mConnectionController->startWriting(exa::OdbcSessionInfoImpl(handle, query));
        if (!writer.expired()) {
            auto writeConnection = std::make_unique<exa::rconnection::RWriterConnection>(writer);
            retVal = writeConnection->create();
            mConnection = std::move(writeConnection);
        }
    }
    return retVal;
}

exa::ProtocolType exa::ConnectionContext::convertProtocol(const char *protocol) {
    exa::ProtocolType retVal = ProtocolType::http;
    if (0 == ::strcmp("http", protocol)) {
        retVal = ProtocolType::http;
    }
    else if (0 == ::strcmp("https", protocol)) {
        retVal = ProtocolType::https;
    } else {
        ::error("Unknown protocol:%s", protocol);
    }
    return retVal;
}

int exa::ConnectionContext::enableTracing(const char *tracefile) {
    return mLogger.enableLogging(tracefile) ? 0 : -1;
}
