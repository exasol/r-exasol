//
// Created by thomas on 14/07/2021.
//

#include <r-exasol/impl/connection/ConnectionFactoryImpl.h>
#include <r-exasol/impl/connection/socket/SocketImpl.h>
#include <r-exasol/impl/connection/protocol/http/reader/HttpChunkReader.h>
#include <r-exasol/impl/connection/protocol/http/writer/HttpChunkWriter.h>

std::unique_ptr<exa::Socket> exa::ConnectionFactoryImpl::createSocket() {
    return std::make_unique<exa::SocketImpl>();
}

std::unique_ptr<exa::reader::Reader> exa::ConnectionFactoryImpl::createHttpReader(Socket& socket) {
    return std::make_unique<exa::reader::HttpChunkReader>(socket, getChunk());
}

std::unique_ptr<exa::writer::Writer> exa::ConnectionFactoryImpl::createHttpWriter(Socket& socket) {
    return std::make_unique<exa::writer::HttpChunkWriter>(socket, getChunk());
}

exa::Chunk & exa::ConnectionFactoryImpl::getChunk() {
    static Chunk chunk;
    return chunk;
}
