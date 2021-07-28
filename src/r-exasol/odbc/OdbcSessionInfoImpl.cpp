//
// Created by thomas on 16/07/2021.
//

#include <r-exasol/odbc/OdbcSessionInfoImpl.h>
#include <r-exasol/odbc/OdbcAsyncExecutorImpl.h>

exa::OdbcSessionInfoImpl::OdbcSessionInfoImpl(pRODBCHandle handle, SQLCHAR *query)
: mHandle(handle)
, mQuery(query) {}

std::unique_ptr <exa::OdbcAsyncExecutor> exa::OdbcSessionInfoImpl::createOdbcAsyncExecutor() const {
    return std::make_unique<exa::OdbcAsyncExecutorImpl>(*this);
}