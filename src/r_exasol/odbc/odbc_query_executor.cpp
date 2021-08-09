#include <r_exasol/odbc/odbc_query_executor.h>
#include <r_exasol/connection/async_executor/async_executor_exception.h>
#include <r_exasol/connection/error_handler.h>

#include <sstream>
#include <utility>

exa::OdbcQueryExecutor::OdbcQueryExecutor(OdbcSessionInfoImpl  odbcSessionInfo)
: mOdbcSessionInfo(std::move(odbcSessionInfo))
, mStmt(nullptr) {}

exa::OdbcQueryExecutor::~OdbcQueryExecutor() {
    if (mStmt != nullptr) {
        (void)::SQLFreeHandle(SQL_HANDLE_STMT, mStmt);
    }
}

bool exa::OdbcQueryExecutor::executeAsyncQuery() {

    mRes = ::SQLExecDirect(mStmt, mOdbcSessionInfo.mQuery, SQL_NTS);
    return (SQL_SUCCESS == mRes || SQL_SUCCESS_WITH_INFO == mRes );
}

void exa::OdbcQueryExecutor::initializeQueryExecutor() {
    ::SQLRETURN res = ::SQLAllocHandle(SQL_HANDLE_STMT, mOdbcSessionInfo.mHandle->hDbc, &mStmt);
    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
        std::stringstream sError;
        sError << "Could not allocate SQLAllocHandle (" << res << ")";
        throw exa::AsyncExecutorException(sError.str());
    }
}

std::string exa::OdbcQueryExecutor::getQueryExecutorResult() {
    std::string errorMsg;
    if (mRes != SQL_SUCCESS && mRes != SQL_SUCCESS_WITH_INFO) {
        ::SQLCHAR sqlstate[6], msg[SQL_MAX_MESSAGE_LENGTH];
        ::SQLINTEGER NativeError;
        ::SQLSMALLINT MsgLen;
        ::SQLRETURN res = ::SQLGetDiagRec(SQL_HANDLE_STMT,
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
