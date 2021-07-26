//
// Created by thomas on 08/07/2021.
//

#include <connection.h>

#include <r-exasol/impl/connection/ConnectionFactoryImpl.h>
#include <r-exasol/impl/connection/ConnectionController.h>
#include <r-exasol/impl/rconnection/RReaderConnection.h>
#include <r-exasol/impl/rconnection/RWriterConnection.h>
#include <r-exasol/impl/odbc/OdbcSessionInfoImpl.h>

namespace exa {
    struct ConnectionContext {
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <ConnectionController> mConnectionController;
    };

    ConnectionFactoryImpl gConnectionFactory;
    ConnectionContext *gpConnectionContext = nullptr;

    void onError(std::string e) {
        ::error(e.c_str());
    }
}


extern "C" {


int initConnection(const char* host, int port) {
    if (exa::gpConnectionContext != nullptr) {
        destroyConnection(0);
    }
    exa::gpConnectionContext = new exa::ConnectionContext();
    exa::gpConnectionContext->mConnectionController =
            std::make_unique<exa::ConnectionController>(exa::gConnectionFactory, &exa::onError);
    return exa::gpConnectionContext->mConnectionController->connect(host, static_cast<uint16_t>(port));
}

int destroyConnection(int closeFd) {
    bool wasDone(false);
    if (exa::gpConnectionContext != nullptr) {
        exa::gpConnectionContext->mConnection->release();
        wasDone = exa::gpConnectionContext->mConnectionController->shutDown();
        delete exa::gpConnectionContext;
        exa::gpConnectionContext = nullptr;
        if(!closeFd && !wasDone) {
            ::warning("Transfer was not done jet.");
        }
    }
    return wasDone ? 0 : -1;
}

SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query) {
    SEXP retVal = nullptr;
    auto connectionContext = exa::gpConnectionContext;
    if (connectionContext != nullptr && connectionContext->mConnectionController) {
        exa::reader::Reader *reader =
                connectionContext->mConnectionController->startReading(exa::OdbcSessionInfoImpl(handle, query),
                                                                       exa::ProtocolType::http);
        if (reader != nullptr) {
            auto readerConnection = std::make_unique<exa::rconnection::RReaderConnection>(*reader);
            retVal = readerConnection->create();
            connectionContext->mConnection = std::move(readerConnection);
        }
    }
    return retVal;
}

SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query) {
    SEXP retVal = nullptr;
    auto connectionContext = exa::gpConnectionContext;
    if (connectionContext != nullptr && connectionContext->mConnectionController) {
        exa::writer::Writer *writer =
                connectionContext->mConnectionController->startWriting(exa::OdbcSessionInfoImpl(handle, query),
                                                                       exa::ProtocolType::http);
        if (writer != nullptr) {
            auto writeConnection = std::make_unique<exa::rconnection::RWriterConnection>(*writer);
            retVal = writeConnection->create();
            connectionContext->mConnection = std::move(writeConnection);
        }
    }
    return retVal;
}

extern SEXP copyHostName() {
    SEXP host = nullptr;
    auto connectionContext = exa::gpConnectionContext;
    if (connectionContext != nullptr && connectionContext->mConnectionController) {
        PROTECT(host = allocVector(STRSXP, 1));
        const std::string hostName = connectionContext->mConnectionController->getHostInfo().first;
        SET_STRING_ELT(host, 0, mkChar(hostName.c_str()));
        UNPROTECT(1);
    }
    return host;
}

extern SEXP copyHostPort() {
    uint16_t hostPort = -1;
    auto connectionContext = exa::gpConnectionContext;
    if (connectionContext != nullptr && connectionContext->mConnectionController) {
        hostPort = connectionContext->mConnectionController->getHostInfo().second;
    }
    return ScalarInteger(hostPort);
}

}
