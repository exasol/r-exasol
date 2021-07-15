//
// Created by thomas on 14/07/2021.
//

#ifndef R_EXASOL_CONNECTION_CONTROLLER_H
#define R_EXASOL_CONNECTION_CONTROLLER_H

#include <if/ConnectionFactory.h>

namespace exa {
    enum ProtocolType {
        http = 0
    };

    class ConnectionController {
    public:
        explicit ConnectionController(ConnectionFactory & connectionFactory);

        reader::Reader* startReading(tSocket, ProtocolType);
        writer::Writer* startWriting(tSocket, ProtocolType);

        void shutDown();

    private:
        ConnectionFactory & mConnectionFactory;
        std::unique_ptr<reader::Reader> mReader;
        std::unique_ptr<writer::Writer> mWriter;
        std::unique_ptr<Socket> mSocket;
    };
}


#endif //R_EXASOL_CONNECTION_CONTROLLER_H
