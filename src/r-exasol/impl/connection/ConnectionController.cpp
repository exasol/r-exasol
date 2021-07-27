//
// Created by thomas on 14/07/2021.
//

#include <r-exasol/impl/connection/ConnectionController.h>
#include <r-exasol/impl/connection/ConnectionException.h>
#include <r-exasol/impl/connection/protocol/metaInfoReader.h>
#include <r-exasol/if/OdbcException.h>


exa::ConnectionController::ConnectionController(ConnectionFactory &connectionFactory, const tErrorFunction & errorHandler)
: mConnectionFactory(connectionFactory)
, mErrorHandler(errorHandler) {}

exa::reader::Reader* exa::ConnectionController::startReading(const OdbcSessionInfo& odbcSessionInfo, ProtocolType protocolType) {
    exa::reader::Reader* retVal = nullptr;
    if (mSocket && protocolType == ProtocolType::http) {
        mOdbcAsyncExecutor = odbcSessionInfo.createOdbcAsyncExecutor();

        try {
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            mReader = mConnectionFactory.createHttpReader(*mSocket);
            mReader->start();
            retVal = mReader.get();
        } catch (const OdbcException & ex) {
            mErrorHandler(ex.what());
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
        try {
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            mWriter = mConnectionFactory.createHttpWriter(*mSocket);
            mWriter->start();
            retVal = mWriter.get();
        } catch (const OdbcException & ex) {
            mErrorHandler(ex.what());
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
    if (mSocket) {
        mSocket->shutdownRdWr();
    }
    mSocket.reset();
    std::string errorMsg;
    if (mOdbcAsyncExecutor) {
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
    if(mSocket) {
        mSocket->shutdownRdWr();
    }
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
