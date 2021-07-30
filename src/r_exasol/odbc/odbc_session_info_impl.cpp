#include <r_exasol/odbc/odbc_session_info_impl.h>
#include <r_exasol/odbc/odbc_async_executor_impl.h>

exa::OdbcSessionInfoImpl::OdbcSessionInfoImpl(::pRODBCHandle handle, ::SQLCHAR *query)
: mHandle(handle)
, mQuery(query) {}

std::unique_ptr <exa::OdbcAsyncExecutor> exa::OdbcSessionInfoImpl::createOdbcAsyncExecutor() const {
    return std::make_unique<exa::OdbcAsyncExecutorImpl>(*this);
}
