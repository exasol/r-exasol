#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer/http_chunk_writer.h>

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
    /**
     * Chunk will be used accross different transfers. Reason is that it's allocation is quiete expansive (@see MAX_HTTP_CHUNK_SIZE)
     * We use a static variable here, which is initialized the first time the function gets invoked,
     * and then remains allocated until the program ends.
     */
    static Chunk chunk;
    return chunk;
}
