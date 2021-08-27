#ifndef R_EXASOL_CONNECTION_CONTROLLER_H
#define R_EXASOL_CONNECTION_CONTROLLER_H

#include <r_exasol/connection/connection_factory.h>
#include <r_exasol/connection/async_executor/async_executor.h>
#include <r_exasol/connection/async_executor/async_executor_session_info.h>
#include <r_exasol/connection/error_handler.h>
#include <r_exasol/connection/connection_info.h>
#include <r_exasol/connection/protocol_type.h>
#include <memory>


namespace exa {
    class ConnectionEstablisher;



    /**
     * The ConnectionController establishes the socket connection, reads the metadata (which then can be access
     * by clients), triggers the asynchronous SQL statement and then creates the reader/writer.
     */
    class ConnectionController {
    public:
        explicit ConnectionController(ConnectionFactory & connectionFactory, const tErrorFunction & errorHandler);

        /**
         * Established connection: Opens sockets and read meta data.
         * @param protocolType Protocol type (Http, Https,...)
         * @param host Database host
         * @param port Host port.
         * @return true if successful, false if an error occurred.
         */
        bool connect(exa::ProtocolType protocolType, const char* host, uint16_t port);

        /**
         * This function triggers the async ODBC statement executor, creates and prepares the reader (for the given protocol).
         * @param sessionInfo Information about the current async execution session.
         * @return Returns reader which can be used for reading user data.
         */
        std::weak_ptr<reader::Reader> startReading(const AsyncExecutorSessionInfo&);
        /**
         * This function triggers the async ODBC statement executor, creates and prepares the writer (for the given protocol).
         * @param sessionInfo Information about the current async execution session.
         * @return Returns reader which can be used for writing user data.
         */
        std::weak_ptr<writer::Writer> startWriting(const AsyncExecutorSessionInfo&);

        void onOdbcError();
        /**
         * Closes connection (Socket/ async Odbc executor / Writer or Reader)
         * @return true if data transfer was finished. false otherwise.
         */
        bool shutDown();

        std::string getProxyHost() const { return mConnectionInfo.proxyHost; }

        uint16_t getProxyPort() const { return mConnectionInfo.proxyPort; }

    private:
        bool isValidProtocol(ProtocolType protocolType);

    private:
        ConnectionFactory & mConnectionFactory;
        std::shared_ptr<reader::Reader> mReader;
        std::shared_ptr<writer::Writer> mWriter;
        std::unique_ptr<AsyncExecutor> mOdbcAsyncExecutor;
        ConnectionInfo mConnectionInfo;
        tErrorFunction mErrorHandler;
    };
}


#endif //R_EXASOL_CONNECTION_CONTROLLER_H
