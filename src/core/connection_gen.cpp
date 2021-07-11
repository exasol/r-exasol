//
// Created by thomas on 08/07/2021.
//

#include <connection_gen.h>
#include <impl/ConnectionContext.h>
#include <map>
#include <impl/socket/SocketImpl.h>
#include <impl/rconnection/RReaderConnection.h>
#include <impl/rconnection/RWriterConnection.h>
#include <impl/transfer/writer/HttpChunkWriter.h>
#include <impl/transfer/reader/HttpChunkReader.h>

using namespace exa;

extern "C" {

    std::map<tSocket, std::unique_ptr<ImportConnectionContext> > gImportConnections;
    std::map<tSocket, std::unique_ptr<ExportConnectionContext> > gExportConnections;

    //TODO: create chunk pool
    Chunk gChunk;

    SEXP createReadConnection(tSocket socketFd) {
        Socket * socket = new SocketImpl(socketFd);
        reader::Reader * reader = new reader::HttpChunkReader(*socket, gChunk);
        auto * rConnection = new rconnection::RReaderConnection(reader);
        auto importConnectionContext = std::make_unique<ImportConnectionContext>(rConnection,reader, socket);
        SEXP retVal = importConnectionContext->mConnection->create();
        gImportConnections[socketFd] = std::move(importConnectionContext);
        return retVal;
    }

    SEXP createWriteConnection(tSocket socketFd) {
        Socket * socket = new SocketImpl(socketFd);
        writer::Writer * writer = new exa::writer::HttpChunkWriter(*socket, gChunk);
        auto * rConnection = new rconnection::RWriterConnection(writer);
        auto exportConnectionContext = std::make_unique<ExportConnectionContext>(rConnection,writer, socket);
        SEXP retVal = exportConnectionContext->mConnection->create();
        gExportConnections[socketFd] = std::move(exportConnectionContext);
        return retVal;
    }

    void destroyReadConnection(tSocket socket) {
        gImportConnections.erase(socket);
    }

    void destroyWriteConnection(tSocket socket) {
        gExportConnections.erase(socket);
    }

}
