//
// Created by thomas on 15/07/2021.
//

#include <r-exasol/impl/odbc/OdbcAsyncExecutorImpl.h>
#include <r-exasol/if/OdbcException.h>

#include <sql.h>
#include <sstream>
#include <utility>

exa::OdbcAsyncExecutorImpl::OdbcAsyncExecutorImpl(OdbcSessionInfoImpl  odbcSessionInfo)
: mOdbcSessionInfo(std::move(odbcSessionInfo))
, mStmt(nullptr) {}

exa::OdbcAsyncExecutorImpl::~OdbcAsyncExecutorImpl() {
    if (mStmt != nullptr) {
        (void)SQLFreeHandle(SQL_HANDLE_STMT, mStmt);
    }
}

void exa::OdbcAsyncExecutorImpl::asyncRODBCQueryExecuter(const tBackgroundOdbcErrorFunction& errorHandler) {

    mRes = SQLExecDirect(mStmt, mOdbcSessionInfo.mQuery, SQL_NTS);
    mDone = true;
    if (mRes != SQL_SUCCESS && mRes != SQL_SUCCESS_WITH_INFO) {
        errorHandler();
    }
}

bool exa::OdbcAsyncExecutorImpl::execute(const tBackgroundOdbcErrorFunction& errorHandler) {
    SQLRETURN res = SQLAllocHandle(SQL_HANDLE_STMT, mOdbcSessionInfo.mHandle->hDbc, &mStmt);

    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
        std::stringstream sError;
        sError << "Could not allocate SQLAllocHandle (" << res << ")";
        throw OdbcException(sError.str());
    }
    mDone = false;
    mThread = std::thread(&OdbcAsyncExecutorImpl::asyncRODBCQueryExecuter, this, errorHandler);
    return true;
}

bool exa::OdbcAsyncExecutorImpl::isDone() {
    return mDone;
}

std::string exa::OdbcAsyncExecutorImpl::joinAndCheckResult() {
    if (mThread.joinable()) {
        mThread.join();
    }
    std::string errorMsg;
    if (mRes != SQL_SUCCESS && mRes != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlstate[6], msg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER NativeError;
        SQLSMALLINT MsgLen;
        SQLRETURN res = SQLGetDiagRec(SQL_HANDLE_STMT,
                            mStmt, 1,
                            sqlstate, &NativeError, msg, sizeof(msg),
                            &MsgLen);
        if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
            errorMsg = "Unknown ODBC error";
        } else {
            std::stringstream sError;
            sError << "Could not execute SQL: '" << sqlstate << "' " << (int)NativeError << "' " << msg << "'";
            errorMsg = sError.str();
        }
    }
    return errorMsg;
}


