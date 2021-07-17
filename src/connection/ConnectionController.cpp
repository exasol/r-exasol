//
// Created by thomas on 14/07/2021.
//

#include <if/ConnectionController.h>
#include <if/ConnectionException.h>
#include <connection//protocol/metaInfoReader.h>

exa::ConnectionController::ConnectionController(ConnectionFactory &connectionFactory, const tErrorFunction & errorHandler)
: mConnectionFactory(connectionFactory)
, mErrorHandler(errorHandler) {}

exa::reader::Reader* exa::ConnectionController::startReading(const OdbcSessionInfo& odbcSessionInfo, ProtocolType protocolType) {
    exa::reader::Reader* retVal = nullptr;
    if (mSocket && protocolType == ProtocolType::http) {
        mOdbcAsyncExecutor = odbcSessionInfo.createOdbcAsyncExecutor();
        mOdbcAsyncExecutor->execute([this](auto && error) { onOdbcError(std::forward<decltype(error)>(error)); });
        mReader = mConnectionFactory.createHttpReader(*mSocket);
        try {
            mReader->start();
            retVal = mReader.get();
        } catch(const ConnectionException & ex) {
            mErrorHandler(ex.what());
        }
    } else if (!mSocket) {
        mErrorHandler("Connection not established");
    } else if (protocolType != ProtocolType::http) {
        mErrorHandler("Protocol not supported");
    }
    return retVal;
}

exa::writer::Writer* exa::ConnectionController::startWriting(const OdbcSessionInfo& odbcSessionInfo, exa::ProtocolType protocolType) {
    exa::writer::Writer* retVal = nullptr;

    if (mSocket && protocolType == ProtocolType::http) {
        mOdbcAsyncExecutor = odbcSessionInfo.createOdbcAsyncExecutor();
        mOdbcAsyncExecutor->execute([this](auto && error) { onOdbcError(std::forward<decltype(error)>(error)); });
        mWriter = mConnectionFactory.createHttpWriter(*mSocket);
        try {
            mWriter->start();
            retVal = mWriter.get();
        } catch(const ConnectionException & ex) {
            mErrorHandler(ex.what());
        }
    } else if (!mSocket) {
        mErrorHandler("Connection not established");
    } else if (protocolType != ProtocolType::http) {
        mErrorHandler("Protocol not supported");
    }
    return retVal;
}

bool exa::ConnectionController::shutDown() {
    bool retVal = true;
    if (mOdbcAsyncExecutor) {
        mOdbcAsyncExecutor->join();
        retVal = mOdbcAsyncExecutor->isDone();
        mOdbcAsyncExecutor.reset();
    }
    mReader.reset();
    mWriter.reset();
    mSocket.reset();
    return retVal;
}

void exa::ConnectionController::onOdbcError(const std::string& error) {
    if(mSocket) {
        mSocket->shutdownRdWr();
    }
    mErrorHandler(error);
}

int exa::ConnectionController::connect(const char *host, uint16_t port) {
    bool success = false;
    mSocket = mConnectionFactory.createSocket();
    try {
        mSocket->connect(host, port);
        mHostInfo = metaInfoReader::read(*mSocket);
        success = true;
    } catch(const ConnectionException& ex) {
        mErrorHandler(ex.what());
        if (mSocket) {
            mSocket->shutdownRdWr();
            mSocket.reset();
        }
    }
    return success ? 0 : -1;
}
