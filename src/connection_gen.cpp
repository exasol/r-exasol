//
// Created by thomas on 08/07/2021.
//

#include "connection_gen.h"

#include <impl/ConnectionFactoryImpl.h>
#include <if/ConnectionController.h>
#include "rconnection/RReaderConnection.h"
#include "rconnection/RWriterConnection.h"

namespace exa {
    struct ConnectionContext {
        std::unique_ptr <exa::rconnection::RConnection> mConnection;
        std::unique_ptr <ConnectionController> mConnectionController;
    };

    ConnectionFactoryImpl gConnectionFactory;
    ConnectionContext *gpConnectionContext = nullptr;
}

extern "C" {


void initImportConnection() {
    if (exa::gpConnectionContext != nullptr) {
        destroyConnection();
    }
    exa::gpConnectionContext = new exa::ConnectionContext();
    exa::gpConnectionContext->mConnectionController = std::make_unique<exa::ConnectionController>(exa::gConnectionFactory);
}

void destroyConnection() {
    if (exa::gpConnectionContext != nullptr) {
        exa::gpConnectionContext->mConnection->release();
        exa::gpConnectionContext->mConnectionController->shutDown();
        delete exa::gpConnectionContext;
        exa::gpConnectionContext = nullptr;
    }
}

SEXP createReadConnection(tSocket socket) {
    SEXP retVal = nullptr;
    auto connectionContext = exa::gpConnectionContext;
    if (connectionContext != nullptr && connectionContext->mConnectionController) {
        exa::reader::Reader *reader = connectionContext->mConnectionController->startReading(socket, exa::ProtocolType::http);
        if (reader != nullptr) {
            auto readerConnection = std::make_unique<exa::rconnection::RReaderConnection>(*reader);
            retVal = readerConnection->create();
            connectionContext->mConnection = std::move(readerConnection);
        }
    }
    return retVal;
}

SEXP createWriteConnection(tSocket socket) {
    SEXP retVal = nullptr;
    auto connectionContext = exa::gpConnectionContext;
    if (connectionContext != nullptr && connectionContext->mConnectionController) {
        exa::writer::Writer *writer = connectionContext->mConnectionController->startWriting(socket, exa::ProtocolType::http);
        if (writer != nullptr) {
            auto writeConnection = std::make_unique<exa::rconnection::RWriterConnection>(*writer);
            retVal = writeConnection->create();
            connectionContext->mConnection = std::move(writeConnection);
        }
    }
    return retVal;
}

}
