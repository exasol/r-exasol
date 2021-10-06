#include <r_exasol/connection/connection_factory_impl.h>
#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <r_exasol/connection/protocol/http/writer/http_chunk_writer.h>
#include <r_exasol/connection/protocol/http/conn/http_connection_establisher.h>
#include <r_exasol/connection/protocol/https/conn/https_connection_establisher.h>
#include <cassert>


exa::ConnectionFactoryImpl::ConnectionFactoryImpl(const tErrorFunction & errorHandler)
: mErrorHandler(errorHandler) {}

std::shared_ptr<exa::reader::Reader> exa::ConnectionFactoryImpl::createHttpReader(std::weak_ptr<Socket> socket) {
    return std::make_shared<exa::reader::HttpChunkReader>(socket, getChunk());
}

std::shared_ptr<exa::writer::Writer> exa::ConnectionFactoryImpl::createHttpWriter(std::weak_ptr<Socket> socket) {
    return std::make_shared<exa::writer::HttpChunkWriter>(socket, getChunk());
}

std::shared_ptr<exa::ConnectionEstablisher> exa::ConnectionFactoryImpl::createConnectionEstablisher(ProtocolType protocolType) {
    std::shared_ptr<ConnectionEstablisher> conn_est;
    switch (protocolType) {
        case http:
            conn_est = std::make_shared<HttpConnectionEstablisher>();
            break;
        case https:
            conn_est = std::make_shared<HttpsConnectionEstablisher>(getCertificate());
            break;
        default:
            mErrorHandler("Unknown protocol. Only Http and Https are supported");
            break;
    }
    return conn_est;
}

exa::Chunk & exa::ConnectionFactoryImpl::getChunk() {
    /**
     * Chunk will be used accross different transfers. Reason is that it's allocation is quite expensive (@see MAX_HTTP_CHUNK_SIZE)
     * We use a static variable here, which is initialized the first time the function gets invoked,
     * and then remains allocated until the program ends.
     * We assume here, that only one read or write can happen at the same time.
     */
    static Chunk chunk;
    return chunk;
}

const exa::ssl::Certificate &exa::ConnectionFactoryImpl::getCertificate() {
    /**
     * The certificate creation is implemented as singleton. This means, it is created once per lifetime of the library.
     * It is no problem if different database nodes (which are the SSL clients) use the same certificate.
     * The certificate will have a duration of 365, so this should also be no problem.
     */
    static ssl::Certificate certificate;
    if(!certificate.isValid()) {
        certificate.mkcert();
    }
    return certificate;
}
