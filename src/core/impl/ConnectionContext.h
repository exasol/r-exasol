//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_CONNECTIONCONTEXT_H
#define R_EXASOL_CONNECTIONCONTEXT_H

#include <impl/socket/Socket.h>
#include <memory>
#include <impl/rconnection/RReaderConnection.h>
#include <impl/rconnection/RWriterConnection.h>
#include <impl/transfer/export/Writer.h>
#include <impl/transfer/import/Reader.h>

namespace exa {

    template<typename ConnectionType, typename TransferType>
    struct ConnectionContext {
        explicit ConnectionContext(ConnectionType * connection, TransferType * transfer, Socket * socket)
        : mConnection(connection)
        , mTransfer(transfer)
        , mSocket(socket) {}
        std::unique_ptr<ConnectionType> mConnection;
        std::unique_ptr<TransferType> mTransfer;
        std::unique_ptr<Socket> mSocket;
    };

    typedef ConnectionContext<rconnection::RReaderConnection, import::Reader> ImportConnectionContext;
    typedef ConnectionContext<rconnection::RWriterConnection, writer::Writer> ExportConnectionContext;

}

#endif //R_EXASOL_CONNECTIONCONTEXT_H
