#include <r-exasol/connection/ConnectionController.h>
#include <r-exasol/connection/ConnectionException.h>
#include <r-exasol/connection/protocol/metaInfoReader.h>
#include <r-exasol/connection/OdbcException.h>


exa::ConnectionController::ConnectionController(ConnectionFactory &connectionFactory, const tErrorFunction & errorHandler)
: mConnectionFactory(connectionFactory)
, mErrorHandler(errorHandler) {}

std::weak_ptr<exa::reader::Reader> exa::ConnectionController::startReading(const OdbcSessionInfo& odbcSessionInfo, ProtocolType protocolType) {
    std::weak_ptr<exa::reader::Reader> retVal;
    if (mSocket && protocolType == ProtocolType::http) {
        mOdbcAsyncExecutor = odbcSessionInfo.createOdbcAsyncExecutor();

        try {
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            mReader = mConnectionFactory.createHttpReader(mSocket);
            mReader->start();
            retVal = mReader;
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

std::weak_ptr<exa::writer::Writer> exa::ConnectionController::startWriting(const OdbcSessionInfo& odbcSessionInfo, exa::ProtocolType protocolType) {
    std::weak_ptr<exa::writer::Writer> retVal;

    if (mSocket && protocolType == ProtocolType::http) {
        mOdbcAsyncExecutor = odbcSessionInfo.createOdbcAsyncExecutor();
        try {
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            mWriter = mConnectionFactory.createHttpWriter(mSocket);
            mWriter->start();
            retVal = mWriter;
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
    //Close socket before joining background thread.
    //In case of writer this is important because the database server will finish the ODBC execution only after the socket has been closed.
    if (mSocket) {
        mSocket->shutdownRdWr();
    }
    mSocket.reset();
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
    if(mSocket) {
        mSocket->shutdownRdWr();
    }
}

bool exa::ConnectionController::connect(const char *host, uint16_t port) {
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
    return success;
}
