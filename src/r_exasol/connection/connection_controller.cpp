#include <r_exasol/connection/connection_controller.h>
#include <r_exasol/connection/connection_exception.h>
#include <r_exasol/connection/async_executor/async_executor_exception.h>
#include <r_exasol/debug_print/debug_printer.h>
#include <r_exasol/connection/connection_establisher.h>

typedef exa::DebugPrinter<exa::ConnectionController> conn_debug_printer;
#define CON_CONTROLLER_STACK_PRINTER STACK_PRINTER( exa::ConnectionController);

exa::ConnectionController::ConnectionController(ConnectionFactory &connectionFactory, const tErrorFunction & errorHandler)
: mConnectionFactory(connectionFactory)
, mErrorHandler(errorHandler) {}

std::weak_ptr<exa::reader::Reader> exa::ConnectionController::startReading(const AsyncExecutorSessionInfo& odbcSessionInfo) {
    CON_CONTROLLER_STACK_PRINTER;
    std::weak_ptr<exa::reader::Reader> retVal;
    if (mConnectionInfo.socket) {
        conn_debug_printer::print("creating asyncExecutor");
        mOdbcAsyncExecutor = odbcSessionInfo.createAsyncExecutor();

        try {
            conn_debug_printer::print("start asyncExecutor");
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            conn_debug_printer::print("creating httpReader");
            mReader = mConnectionFactory.createHttpReader(mConnectionInfo.socket);
            conn_debug_printer::print("starting httpReader");
            mReader->start();
            retVal = mReader;
        } catch (const AsyncExecutorException & ex) {
            conn_debug_printer::print("Catched  AsyncExecutorException:", ex.what());
            mErrorHandler(ex.what());
        } catch(const ConnectionException & ex) {
            conn_debug_printer::print("Catched  ConnectionException:", ex.what());
            mErrorHandler(ex.what());
        }
    } else if (!mConnectionInfo.socket) {
        mErrorHandler("Connection not established");
    }
    return retVal;
}

std::weak_ptr<exa::writer::Writer> exa::ConnectionController::startWriting(const AsyncExecutorSessionInfo& odbcSessionInfo) {
    CON_CONTROLLER_STACK_PRINTER;
    std::weak_ptr<exa::writer::Writer> retVal;
    if (mConnectionInfo.socket) {
        conn_debug_printer::print("creating asyncExecutor");
        mOdbcAsyncExecutor = odbcSessionInfo.createAsyncExecutor();
        try {
            conn_debug_printer::print("start asyncExecutor");
            mOdbcAsyncExecutor->execute([this]() { onOdbcError(); });
            conn_debug_printer::print("creating httpWriter");
            mWriter = mConnectionFactory.createHttpWriter(mConnectionInfo.socket);
            conn_debug_printer::print("starting httpWriter");
            mWriter->start();
            retVal = mWriter;
        } catch (const AsyncExecutorException & ex) {
            conn_debug_printer::print("Catched  AsyncExecutorException:", ex.what());
            mErrorHandler(ex.what());
        } catch(const ConnectionException & ex) {
            conn_debug_printer::print("Catched  ConnectionException:", ex.what());
            mErrorHandler(ex.what());
        }
    } else if (!mConnectionInfo.socket) {
        mErrorHandler("Connection not established");
    }
    return retVal;
}

bool exa::ConnectionController::shutDown() {
    CON_CONTROLLER_STACK_PRINTER;
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
    CON_CONTROLLER_STACK_PRINTER;
    if(mConnectionInfo.socket) {
        mConnectionInfo.socket->shutdownRdWr();
    }
}

bool exa::ConnectionController::connect(exa::ProtocolType protocolType, const char *host, uint16_t port) {
    CON_CONTROLLER_STACK_PRINTER;
    bool success = false;
    if (isValidProtocol(protocolType)) {
        std::shared_ptr<ConnectionEstablisher> conn_est = mConnectionFactory.createConnectionEstablisher(protocolType);
        if (conn_est) {
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
    }
    return success;
}

bool exa::ConnectionController::isValidProtocol(exa::ProtocolType protocolType) {
    return ProtocolType::http == protocolType || ProtocolType::https == protocolType;
}


