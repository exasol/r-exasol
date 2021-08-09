#include <r_exasol/odbc/odbc_session_info_impl.h>
#include <r_exasol/odbc/odbc_query_executor.h>
#include <r_exasol/connection/async_executor/async_executor_impl.h>

exa::OdbcSessionInfoImpl::OdbcSessionInfoImpl(::pRODBCHandle handle, ::SQLCHAR *query)
: mHandle(handle)
, mQuery(query) {}

std::unique_ptr<exa::AsyncExecutor> exa::OdbcSessionInfoImpl::createAsyncExecutor() const {
    return std::make_unique< exa::AsyncExecutorImpl<OdbcQueryExecutor, OdbcSessionInfoImpl> >(*this);
}
