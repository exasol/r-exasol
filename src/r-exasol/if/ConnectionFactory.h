//
// Created by thomas on 14/07/2021.
//

#ifndef R_EXASOL_CONNECTIONFACTORY_H
#define R_EXASOL_CONNECTIONFACTORY_H

#include <r-exasol/if/ExaTypes.h>
#include <r-exasol/if/Reader.h>
#include <r-exasol/if/Writer.h>
#include <r-exasol/if/Socket.h>
#include <memory>

namespace exa {

    class ConnectionFactory {
    public:
        virtual ~ConnectionFactory() = default;
        virtual std::unique_ptr<Socket> createSocket() = 0;
        virtual std::unique_ptr<reader::Reader> createHttpReader(Socket&) = 0;
        virtual std::unique_ptr<writer::Writer> createHttpWriter(Socket&) = 0;
    };
}

#endif //R_EXASOL_CONNECTIONFACTORY_H
