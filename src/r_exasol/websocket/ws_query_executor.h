#ifndef R_EXASOL_WS_QUERY_EXECUTOR_H
#define R_EXASOL_WS_QUERY_EXECUTOR_H

#include <r_exasol/connection/async_executor/query_executor.h>
#include <r_exasol/websocket/ws_session_info_impl.h>
#include <r_exasol/debug_print/debug_printer.h>
#include <string>

namespace exa {

    /// Implements QueryExecutor by sending the EXPORT/IMPORT query via WebSocket.
    class WsQueryExecutor : public QueryExecutor {
    public:
        explicit WsQueryExecutor(WsSessionInfoImpl sessionInfo);

        void initializeQueryExecutor() override;
        std::string getQueryExecutorResult() override;
        bool executeAsyncQuery() override;

    private:
        ObjectLifecycleLogger<WsQueryExecutor> mObjectLifecycleLogger;
        WsSessionInfoImpl mSessionInfo;
        std::string mErrorMessage;
    };
}

#endif // R_EXASOL_WS_QUERY_EXECUTOR_H
