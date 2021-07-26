//
// Created by thomas on 14/07/2021.
//

#ifndef R_EXASOL_CONNECTIONFACTORYIMPL_H
#define R_EXASOL_CONNECTIONFACTORYIMPL_H

#include <r-exasol/impl/connection/ConnectionFactory.h>

namespace exa {

    class Chunk;

    class ConnectionFactoryImpl : public ConnectionFactory {
        std::unique_ptr<Socket> createSocket() override;
        std::unique_ptr<reader::Reader> createHttpReader(Socket&) override;
        std::unique_ptr<writer::Writer> createHttpWriter(Socket&) override;

    private:
        Chunk & getChunk();
    };
}


#endif //R_EXASOL_CONNECTIONFACTORYIMPL_H
