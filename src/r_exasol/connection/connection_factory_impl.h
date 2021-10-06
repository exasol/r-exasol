#ifndef R_EXASOL_CONNECTION_FACTORY_IMPL_H
#define R_EXASOL_CONNECTION_FACTORY_IMPL_H

#include <r_exasol/connection/connection_factory.h>
#include <r_exasol/connection/error_handler.h>

namespace exa {

    struct Chunk;
    namespace ssl {
        class Certificate;
    }

    /**
     * Concrete implementation of the connection factory which provides instance creation of sockets, reader and writer.
     * The factory manages the memory buffer used by the reader/writer to exchange data between the network connection and the client.
     */
    class ConnectionFactoryImpl : public ConnectionFactory {
    public:
        ConnectionFactoryImpl(const tErrorFunction & errorHandler);
        std::shared_ptr<reader::Reader> createHttpReader(std::weak_ptr<Socket>) override;
        std::shared_ptr<writer::Writer> createHttpWriter(std::weak_ptr<Socket>) override;
        std::shared_ptr<ConnectionEstablisher> createConnectionEstablisher(ProtocolType protocolType) override;

    private:
        Chunk & getChunk();
        const exa::ssl::Certificate& getCertificate();
        tErrorFunction mErrorHandler;
    };
}


#endif //R_EXASOL_CONNECTION_FACTORY_IMPL_H
