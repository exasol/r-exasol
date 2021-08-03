#ifndef R_EXASOL_CUSTOMASYNCSESSIONMOCK_H
#define R_EXASOL_CUSTOMASYNCSESSIONMOCK_H

#include <r_exasol/connection/error_handler.h>
#include <r_exasol/connection/async_executor/async_executor_session_info.h>
#include <memory>
#include "CustomMockFunction.h"

struct CustomAsyncSessionMock : public exa::AsyncExecutorSessionInfo {
    explicit  CustomAsyncSessionMock(tAsyncFunction asyncFunction);
    std::unique_ptr<exa::AsyncExecutor> createAsyncExecutor() const override;
    tAsyncFunction asyncFunction;
};




#endif //R_EXASOL_CUSTOMASYNCSESSIONMOCK_H
