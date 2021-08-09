#include "AsyncSessionMock.h"
#include "AsyncExecutorMock.h"
#include <r_exasol/connection/async_executor/async_executor_impl.h>
#include <memory>

AsyncSessionMock::AsyncSessionMock(bool & joinCalled)
: joinCalled(joinCalled) {}

std::unique_ptr<exa::AsyncExecutor> AsyncSessionMock::createAsyncExecutor() const  {
    return std::make_unique< exa::AsyncExecutorImpl<AsyncExecutorMock, AsyncSessionMock> >  (*this);
}
