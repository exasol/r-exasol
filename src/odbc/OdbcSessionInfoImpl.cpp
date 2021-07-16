//
// Created by thomas on 16/07/2021.
//

#include "OdbcSessionInfoImpl.h"
#include "OdbcAsyncExecutorImpl.h"

exa::OdbcSessionInfoImpl::OdbcSessionInfoImpl(pRODBCHandle handle, SQLCHAR *query)
: mHandle(handle)
, mQuery(query) {}

std::unique_ptr <exa::OdbcAsyncExecutor> exa::OdbcSessionInfoImpl::createOdbcAsyncExecutor() const {
    return std::make_unique<exa::OdbcAsyncExecutorImpl>(*this);
}
