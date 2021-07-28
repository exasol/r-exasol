//
// Created by thomas on 14/07/2021.
//

#include <r-exasol/connection/ConnectionFactoryImpl.h>
#include <r-exasol/connection/socket/SocketImpl.h>
#include <r-exasol/connection/protocol/http/reader/HttpChunkReader.h>
#include <r-exasol/connection/protocol/http/writer/HttpChunkWriter.h>

std::shared_ptr<exa::Socket> exa::ConnectionFactoryImpl::createSocket() {
    return std::make_shared<exa::SocketImpl>();
}

std::shared_ptr<exa::reader::Reader> exa::ConnectionFactoryImpl::createHttpReader(std::weak_ptr<Socket> socket) {
    return std::make_shared<exa::reader::HttpChunkReader>(socket, getChunk());
}

std::shared_ptr<exa::writer::Writer> exa::ConnectionFactoryImpl::createHttpWriter(std::weak_ptr<Socket> socket) {
    return std::make_shared<exa::writer::HttpChunkWriter>(socket, getChunk());
}

exa::Chunk & exa::ConnectionFactoryImpl::getChunk() {
    static Chunk chunk;
    return chunk;
}
