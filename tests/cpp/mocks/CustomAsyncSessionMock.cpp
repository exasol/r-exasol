#include <r_exasol/connection/async_executor/async_executor_impl.h>
#include "CustomAsyncSessionMock.h"
#include "CustomAsyncExecutorMock.h"

CustomAsyncSessionMock::CustomAsyncSessionMock(tAsyncFunction asyncFunction)
: asyncFunction(asyncFunction) {}

std::unique_ptr<exa::AsyncExecutor> CustomAsyncSessionMock::createAsyncExecutor() const {
    return std::make_unique< exa::AsyncExecutorImpl<CustomAsyncExecutorMock, CustomAsyncSessionMock> >  (*this);
}
