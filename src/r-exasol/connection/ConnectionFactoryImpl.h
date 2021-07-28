//
// Created by thomas on 14/07/2021.
//

#ifndef R_EXASOL_CONNECTIONFACTORYIMPL_H
#define R_EXASOL_CONNECTIONFACTORYIMPL_H

#include <r-exasol/connection/ConnectionFactory.h>

namespace exa {

    class Chunk;

    class ConnectionFactoryImpl : public ConnectionFactory {
        std::shared_ptr<Socket> createSocket() override;
        std::shared_ptr<reader::Reader> createHttpReader(std::weak_ptr<Socket>) override;
        std::shared_ptr<writer::Writer> createHttpWriter(std::weak_ptr<Socket>) override;

    private:
        Chunk & getChunk();
    };
}


#endif //R_EXASOL_CONNECTIONFACTORYIMPL_H
