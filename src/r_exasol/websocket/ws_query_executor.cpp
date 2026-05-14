#include <r_exasol/websocket/ws_query_executor.h>
#include <r_exasol/websocket/exasol_commands.h>
#include <r_exasol/debug_print/debug_printer.h>

typedef exa::DebugPrinter<exa::WsQueryExecutor> ws_debug_printer;
#define WQE_STACK_PRINTER STACK_PRINTER(exa::WsQueryExecutor);

exa::WsQueryExecutor::WsQueryExecutor(WsSessionInfoImpl sessionInfo)
    : mSessionInfo(std::move(sessionInfo)) {}

void exa::WsQueryExecutor::initializeQueryExecutor() {
    WQE_STACK_PRINTER;
    ws_debug_printer::print("initializeQueryExecutor (no-op for WebSocket)");
}

bool exa::WsQueryExecutor::executeAsyncQuery() {
    WQE_STACK_PRINTER;
    ws_debug_printer::print("executeAsyncQuery via WebSocket, query=", mSessionInfo.mQuery.c_str());
    try {
        mSessionInfo.mCmds->execute(mSessionInfo.mQuery);
        ws_debug_printer::print("executeAsyncQuery finished successfully");
        return true;
    } catch (const std::exception& ex) {
        mErrorMessage = ex.what();
        ws_debug_printer::print("executeAsyncQuery failed:", mErrorMessage.c_str());
        return false;
    }
}

std::string exa::WsQueryExecutor::getQueryExecutorResult() {
    WQE_STACK_PRINTER;
    return mErrorMessage;
}
