#ifndef R_EXASOL_CONNECTION_FACTORY_IMPL_H
#define R_EXASOL_CONNECTION_FACTORY_IMPL_H

#include <r-exasol/connection/connection_factory.h>

namespace exa {

    class Chunk;

    /**
     * Concrete implementation of the connection factory which provides instance creation of sockets, reader and writer.
     * The factory manages the memory buffer used by the reader/writer to exchange data between the network connection and the client.
     */
    class ConnectionFactoryImpl : public ConnectionFactory {
        std::shared_ptr<Socket> createSocket() override;
        std::shared_ptr<reader::Reader> createHttpReader(std::weak_ptr<Socket>) override;
        std::shared_ptr<writer::Writer> createHttpWriter(std::weak_ptr<Socket>) override;

    private:
        Chunk & getChunk();
    };
}


#endif //R_EXASOL_CONNECTION_FACTORY_IMPL_H
