#ifndef R_EXASOL_CONNECTION_FACTORY_H
#define R_EXASOL_CONNECTION_FACTORY_H

#include  <r-exasol/connection/reader.h>
#include  <r-exasol/connection/writer.h>
#include  <r-exasol/connection/socket/socket.h>
#include <memory>

namespace exa {

    /**
     * Abstract interface which provides instance creation of sockets, reader and writer.
     */
    class ConnectionFactory {
    public:
        virtual ~ConnectionFactory() = default;
        virtual std::shared_ptr<Socket> createSocket() = 0;
        virtual std::shared_ptr<reader::Reader> createHttpReader(std::weak_ptr<Socket>) = 0;
        virtual std::shared_ptr<writer::Writer> createHttpWriter(std::weak_ptr<Socket>) = 0;
    };
}

#endif //R_EXASOL_CONNECTION_FACTORY_H
