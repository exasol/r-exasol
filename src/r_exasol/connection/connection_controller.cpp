#include <r_exasol/connection/connection_controller.h>
#include <r_exasol/connection/connection_exception.h>
#include <r_exasol/connection/protocol/meta_info_reader.h>
#include <r_exasol/connection/async_executor/async_executor_exception.h>
#include <cassert>
#include <r_exasol/connection/protocol/http/conn/http_connection_establisher.h>
#include <r_exasol/connection/protocol/https/conn/https_connection_establisher.h>
#include <thread>
#include "connection_establisher.h"


exa::ConnectionController::ConnectionController(ConnectionFactory &connectionFactory, const tErrorFunction & errorHandler)
: mConnectionFactory(connectionFactory)
, mErrorHandler(errorHandler) {}

std::weak_ptr<exa::reader::Reader> exa::ConnectionController::startReading(const AsyncExecutorSessionInfo& odbcSessionInfo,
                                                                           ProtocolType protocolType) {
    std::weak_ptr<exa::reader::Reader> retVal;
    if (mConnectionInfo.socket && isValidProtocol(protocolType)) {
        mOdbcAsyncExecutor = odbcSessionInfo.createAsyncExecutor();

        try {
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            mReader = mConnectionFactory.createHttpReader(mConnectionInfo.socket);
            mReader->start();
            retVal = mReader;
        } catch (const AsyncExecutorException & ex) {
            mErrorHandler(ex.what());
        } catch(const ConnectionException & ex) {
            mErrorHandler(ex.what());
        }
    } else if (!mConnectionInfo.socket) {
        mErrorHandler("Connection not established");
    } else if (protocolType != ProtocolType::http) {
        mErrorHandler("Protocol not supported");
    }
    return retVal;
}

std::weak_ptr<exa::writer::Writer> exa::ConnectionController::startWriting(const AsyncExecutorSessionInfo& odbcSessionInfo,
                                                                           exa::ProtocolType protocolType) {
    std::weak_ptr<exa::writer::Writer> retVal;

    if (mConnectionInfo.socket && isValidProtocol(protocolType)) {
        mOdbcAsyncExecutor = odbcSessionInfo.createAsyncExecutor();
        try {
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            mWriter = mConnectionFactory.createHttpWriter(mConnectionInfo.socket);
            mWriter->start();
            retVal = mWriter;
        } catch (const AsyncExecutorException & ex) {
            mErrorHandler(ex.what());
        } catch(const ConnectionException & ex) {
            mErrorHandler(ex.what());
        }
    } else if (!mConnectionInfo.socket) {
        mErrorHandler("Connection not established");
    } else if (protocolType != ProtocolType::http) {
        mErrorHandler("Protocol not supported");
    }
    return retVal;
}

bool exa::ConnectionController::shutDown() {
    bool retVal = true;
    //Close socket before joining background thread.
    //In case of writer this is important because the database server will finish the ODBC execution only after the socket has been closed.
    if (mConnectionInfo.socket) {
        mConnectionInfo.socket->shutdownRdWr();
    }
    mConnectionInfo.socket.reset();
    std::string errorMsg;
    if (mOdbcAsyncExecutor) {
        //Join background thread and get result
        errorMsg = mOdbcAsyncExecutor->joinAndCheckResult();
        retVal = mOdbcAsyncExecutor->isDone();
        mOdbcAsyncExecutor.reset();
    }
    mReader.reset();
    mWriter.reset();

    if (!errorMsg.empty()) {
        mErrorHandler(errorMsg);
    }
    return retVal;
}

void exa::ConnectionController::onOdbcError() {
    if(mConnectionInfo.socket) {
        mConnectionInfo.socket->shutdownRdWr();
    }
}

bool exa::ConnectionController::connect(exa::ProtocolType protocolType, const char *host, uint16_t port) {
    bool success = false;
    if (isValidProtocol(protocolType)) {
        std::unique_ptr<ConnectionEstablisher> conn_est = getConnectionEstablisher(protocolType);

        try {
            mConnectionInfo = conn_est->connect(host, port);
            success = true;
        } catch(const ConnectionException& ex) {
            mErrorHandler(ex.what());
            if (mConnectionInfo.socket) {
                mConnectionInfo.socket->shutdownRdWr();
                mConnectionInfo.socket.reset();
            }
        }
    }
    return success;
}

std::unique_ptr<exa::ConnectionEstablisher>
exa::ConnectionController::getConnectionEstablisher(const exa::ProtocolType &protocolType) const {
    std::unique_ptr<ConnectionEstablisher> conn_est;
    switch (protocolType) {
        case http:
            conn_est = std::make_unique<HttpConnectionEstablisher>();
            break;
        case https:
            conn_est = std::make_unique<HttpsConnectionEstablisher>();
            break;
        default:
            assert(false);
    }
    return conn_est;
}

bool exa::ConnectionController::isValidProtocol(exa::ProtocolType protocolType) {
    return ProtocolType::http == protocolType || ProtocolType::https == protocolType;
}


