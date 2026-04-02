#include <r_exasol/websocket/ws_session_info_impl.h>

#include <utility>
#include <r_exasol/websocket/ws_query_executor.h>
#include <r_exasol/connection/async_executor/async_executor_impl.h>

exa::WsSessionInfoImpl::WsSessionInfoImpl(ExasolCommands* cmds, std::string query)
    : mCmds(cmds)
    , mQuery(std::move(query)) {}

std::unique_ptr<exa::AsyncExecutor> exa::WsSessionInfoImpl::createAsyncExecutor() const {
    return std::make_unique<exa::AsyncExecutorImpl<WsQueryExecutor, WsSessionInfoImpl>>(*this);
}
