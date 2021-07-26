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
        ConnectionContext() = default;
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <ConnectionController> mConnectionController;
    };

    ConnectionFactoryImpl gConnectionFactory;
    std::unique_ptr<ConnectionContext> gpConnectionContext;

    void onError(std::string e) {
        ::error(e.c_str());
    }
}

extern "C" {


int initConnection(const char* host, int port) {
    if (exa::gpConnectionContext) {
        destroyConnection(0);
    }
    exa::gpConnectionContext = std::make_unique<exa::ConnectionContext>();
    exa::gpConnectionContext->mConnectionController =
            std::make_unique<exa::ConnectionController>(exa::gConnectionFactory, exa::onError);
    return exa::gpConnectionContext->mConnectionController->connect(host, static_cast<uint16_t>(port));
}

int destroyConnection(int closeFd) {
    bool wasDone(false);
    if (exa::gpConnectionContext) {
        if (exa::gpConnectionContext->mConnection) {
            exa::gpConnectionContext->mConnection->release();
        }
        if (exa::gpConnectionContext->mConnectionController) {
            wasDone = exa::gpConnectionContext->mConnectionController->shutDown();
        }
        exa::gpConnectionContext.reset();
        if(!closeFd && !wasDone) {
            ::warning("Transfer was not done jet.");
        }
    }
    return wasDone ? 0 : -1;
}

SEXP createReadConnection(pRODBCHandle handle, SQLCHAR *query) {
    SEXP retVal = nullptr;
    if (exa::gpConnectionContext && exa::gpConnectionContext->mConnectionController) {
        exa::reader::Reader *reader =
                exa::gpConnectionContext->mConnectionController->startReading(exa::OdbcSessionInfoImpl(handle, query),
                                                                                exa::ProtocolType::http);
        if (reader != nullptr) {
            auto readerConnection = std::make_unique<exa::rconnection::RReaderConnection>(*reader);
            retVal = readerConnection->create();
            exa::gpConnectionContext->mConnection = std::move(readerConnection);
        }
    }
    return retVal;
}

SEXP createWriteConnection(pRODBCHandle handle, SQLCHAR *query) {
    SEXP retVal = nullptr;
    if (exa::gpConnectionContext && exa::gpConnectionContext->mConnectionController) {
        exa::writer::Writer *writer =
                exa::gpConnectionContext->mConnectionController->startWriting(exa::OdbcSessionInfoImpl(handle, query),
                                                                               exa::ProtocolType::http);
        if (writer != nullptr) {
            auto writeConnection = std::make_unique<exa::rconnection::RWriterConnection>(*writer);
            retVal = writeConnection->create();
            exa::gpConnectionContext->mConnection = std::move(writeConnection);
        }
    }
    return retVal;
}

extern SEXP copyHostName() {
    SEXP host = nullptr;
    if (exa::gpConnectionContext && exa::gpConnectionContext->mConnectionController) {
        PROTECT(host = allocVector(STRSXP, 1));
        const std::string hostName = exa::gpConnectionContext->mConnectionController->getHostInfo().first;
        SET_STRING_ELT(host, 0, mkChar(hostName.c_str()));
        UNPROTECT(1);
    }
    return host;
}

extern SEXP copyHostPort() {
    uint16_t hostPort = -1;
    if (exa::gpConnectionContext && exa::gpConnectionContext->mConnectionController) {
        hostPort = exa::gpConnectionContext->mConnectionController->getHostInfo().second;
    }
    return ScalarInteger(hostPort);
}

}
