//
// Created by thomas on 14/07/2021.
//

#ifndef R_EXASOL_CONNECTION_CONTROLLER_H
#define R_EXASOL_CONNECTION_CONTROLLER_H

#include <r-exasol/connection/ConnectionFactory.h>
#include <r-exasol/connection/OdbcAsyncExecutor.h>
#include <r-exasol/connection/OdbcSessionInfo.h>
#include <r-exasol/connection/ErrorHandler.h>
#include <memory>

namespace exa {
    enum ProtocolType {
        http = 0
    };

    class ConnectionController {
    public:
        explicit ConnectionController(ConnectionFactory & connectionFactory, const tErrorFunction & errorHandler);

        int connect(const char* host, uint16_t port);

        std::weak_ptr<reader::Reader> startReading(const OdbcSessionInfo&, ProtocolType);
        std::weak_ptr<writer::Writer> startWriting(const OdbcSessionInfo&,ProtocolType);

        void onOdbcError();
        /// Shuts connection controller down.
        /// \return Returns true if transfer was finished. false otherwise.
        bool shutDown();

        std::pair<std::string, uint16_t > getHostInfo() const { return mHostInfo; }

    private:
        ConnectionFactory & mConnectionFactory;
        std::shared_ptr<reader::Reader> mReader;
        std::shared_ptr<writer::Writer> mWriter;
        std::shared_ptr<OdbcAsyncExecutor> mOdbcAsyncExecutor;
        std::shared_ptr<Socket> mSocket;
        std::pair<std::string, uint16_t > mHostInfo;
        tErrorFunction mErrorHandler;
    };
}


#endif //R_EXASOL_CONNECTION_CONTROLLER_H
