//
// Created by thomas on 15/07/2021.
//

#include <odbc/OdbcAsyncExecutorImpl.h>

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

void exa::OdbcAsyncExecutorImpl::asyncRODBCQueryExecuter(const tErrorFunction& errorHandler) {

    SQLRETURN res = SQLExecDirect(mStmt, mOdbcSessionInfo.mQuery, SQL_NTS);
    mDone = true;
    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlstate[6], msg[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER NativeError;
        SQLSMALLINT MsgLen;
        res =  SQLGetDiagRec(SQL_HANDLE_STMT,
                                mStmt, 1,
                                sqlstate, &NativeError, msg, sizeof(msg),
                                &MsgLen);
        if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
            errorHandler("Unknown ODBC error");
        } else {
            std::stringstream sError;
            sError << "Could not execute SQL: '" << sqlstate << "' " << (int)NativeError << "' " << msg << "'";
            errorHandler(sError.str());
        }
    }
}

bool exa::OdbcAsyncExecutorImpl::execute(const tErrorFunction& errorHandler) {
    SQLRETURN res = SQLAllocHandle(SQL_HANDLE_STMT, mOdbcSessionInfo.mHandle->hDbc, &mStmt);
    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
        std::stringstream sError;
        sError << "Could not allocate SQLAllocHandle (" << res << ")";
        errorHandler(sError.str());
        return false;
    }
    mDone = false;
    mThread = std::thread(&OdbcAsyncExecutorImpl::asyncRODBCQueryExecuter, this, errorHandler);
    return true;
}

bool exa::OdbcAsyncExecutorImpl::isDone() {
    return mDone;
}

void exa::OdbcAsyncExecutorImpl::join() {
    if (mThread.joinable()) {
        mThread.join();
    }
}


