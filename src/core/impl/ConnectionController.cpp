//
// Created by thomas on 14/07/2021.
//

#include <if/ConnectionController.h>

exa::ConnectionController::ConnectionController(ConnectionFactory &connectionFactory)
: mConnectionFactory(connectionFactory) {

}

exa::reader::Reader* exa::ConnectionController::startReading(tSocket socket, ProtocolType protocolType) {
    return nullptr;
}

exa::writer::Writer *exa::ConnectionController::startWriting(tSocket, exa::ProtocolType) {
    return nullptr;
}

void exa::ConnectionController::shutDown() {

}
