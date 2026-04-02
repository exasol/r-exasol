#ifndef R_EXASOL_WS_SESSION_INFO_IMPL_H
#define R_EXASOL_WS_SESSION_INFO_IMPL_H

#include <r_exasol/connection/async_executor/async_executor_session_info.h>
#include <string>

namespace exa {
    class ExasolCommands;

    /// AsyncExecutorSessionInfo implementation backed by a WebSocket session.
    struct WsSessionInfoImpl : public AsyncExecutorSessionInfo {
        WsSessionInfoImpl(ExasolCommands* cmds, std::string query);

        std::unique_ptr<AsyncExecutor> createAsyncExecutor() const override;

        ExasolCommands* mCmds;
        std::string mQuery;
    };
}

#endif // R_EXASOL_WS_SESSION_INFO_IMPL_H
