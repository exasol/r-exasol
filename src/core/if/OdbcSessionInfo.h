//
// Created by thomas on 16/07/2021.
//

#ifndef R_EXASOL_ODBCSESSIONINFO_H
#define R_EXASOL_ODBCSESSIONINFO_H

#include <memory>

namespace exa {
    class OdbcAsyncExecutor;

    struct OdbcSessionInfo {
        virtual ~OdbcSessionInfo() = default;
        virtual std::unique_ptr<OdbcAsyncExecutor> createOdbcAsyncExecutor() const = 0;
    };
}
#endif //R_EXASOL_ODBCSESSIONINFO_H
