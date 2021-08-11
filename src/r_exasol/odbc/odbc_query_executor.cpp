#include <r_exasol/odbc/odbc_query_executor.h>
#include <r_exasol/connection/async_executor/async_executor_exception.h>
#include <r_exasol/connection/error_handler.h>

#include <sstream>
#include <utility>

#include <r_exasol/debug_print/debug_printer.h>

typedef exa::DebugPrinter<exa::OdbcQueryExecutor> odbc_debug_printer;
typedef exa::StackTraceLogger<exa::OdbcQueryExecutor> odbc_stack_trace_logger;


exa::OdbcQueryExecutor::OdbcQueryExecutor(OdbcSessionInfoImpl  odbcSessionInfo)
: mOdbcSessionInfo(std::move(odbcSessionInfo))
, mStmt(nullptr) {}

exa::OdbcQueryExecutor::~OdbcQueryExecutor() {
    if (mStmt != nullptr) {
        (void)::SQLFreeHandle(SQL_HANDLE_STMT, mStmt);
    }
}

bool exa::OdbcQueryExecutor::executeAsyncQuery() {
    odbc_stack_trace_logger ostl("executeAsyncQuery");
    mRes = ::SQLExecDirect(mStmt, mOdbcSessionInfo.mQuery, SQL_NTS);
    odbc_debug_printer::print("executeAsyncQuery finished with result = ", static_cast<int>(mRes));
    return (SQL_SUCCESS == mRes || SQL_SUCCESS_WITH_INFO == mRes );
}

void exa::OdbcQueryExecutor::initializeQueryExecutor() {
    odbc_stack_trace_logger ostl("initializeQueryExecutor");
    odbc_debug_printer::print("mOdbcSessionInfo.mHandle: hDbc=", mOdbcSessionInfo.mHandle->hDbc,
                              " hStmt=", mOdbcSessionInfo.mHandle->hStmt,
                              " nRows=", mOdbcSessionInfo.mHandle->nRows,
                              " nColumns=", mOdbcSessionInfo.mHandle->nColumns,
                              " channel=", mOdbcSessionInfo.mHandle->channel,
                              " id=", mOdbcSessionInfo.mHandle->id,
                              " useNRows=", mOdbcSessionInfo.mHandle->useNRows,
                              " ColData=", mOdbcSessionInfo.mHandle->ColData,
                              " nAllocated=", mOdbcSessionInfo.mHandle->nAllocated,
                              " rowsFetched=", mOdbcSessionInfo.mHandle->rowsFetched,
                              " rowArraySize=", mOdbcSessionInfo.mHandle->rowArraySize,
                              " rowsUsed=", mOdbcSessionInfo.mHandle->rowsUsed,
                              " msglist=", mOdbcSessionInfo.mHandle->msglist,
                              " extPtr=", mOdbcSessionInfo.mHandle->extPtr);
    ::SQLRETURN res = ::SQLAllocHandle(SQL_HANDLE_STMT, mOdbcSessionInfo.mHandle->hDbc, &mStmt);
    odbc_debug_printer::print("::SQLAllocHandle finished with result = ", static_cast<int>(res));
    if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
        std::stringstream sError;
        sError << "Could not allocate SQLAllocHandle (" << res << ")";
        throw exa::AsyncExecutorException(sError.str());
    }
}

std::string exa::OdbcQueryExecutor::getQueryExecutorResult() {
    odbc_stack_trace_logger ostl("getQueryExecutorResult");
    std::string errorMsg;
    if (mRes != SQL_SUCCESS && mRes != SQL_SUCCESS_WITH_INFO) {
        odbc_debug_printer::print("mRes indicates an error...");
        ::SQLCHAR sqlstate[6], msg[SQL_MAX_MESSAGE_LENGTH];
        ::SQLINTEGER NativeError;
        ::SQLSMALLINT MsgLen;
        ::SQLRETURN res = ::SQLGetDiagRec(SQL_HANDLE_STMT,
                            mStmt, 1,
                            sqlstate, &NativeError, msg, sizeof(msg),
                            &MsgLen);
        if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
            odbc_debug_printer::print("Unknown ODBC error");
            errorMsg = "Unknown ODBC error";
        } else {
            std::stringstream sError;
            sError << "Could not execute SQL: '" << sqlstate << "' " << (int)NativeError << "' " << msg << "'";
            errorMsg = sError.str();
            odbc_debug_printer::print("ODBC error:", errorMsg.c_str());
        }
    }
    return errorMsg;
}
