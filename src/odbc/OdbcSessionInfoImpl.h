//
// Created by thomas on 16/07/2021.
//

#ifndef R_EXASOL_ODBCSESSIONINFOIMPL_H
#define R_EXASOL_ODBCSESSIONINFOIMPL_H

#include <if/OdbcSessionInfo.h>
#include <sql.h>
#include "RODBC.h"


namespace exa {
    struct OdbcSessionInfoImpl : public OdbcSessionInfo {
        OdbcSessionInfoImpl(pRODBCHandle handle, SQLCHAR *query);
        std::unique_ptr<OdbcAsyncExecutor> createOdbcAsyncExecutor() const override;
        pRODBCHandle mHandle;
        SQLCHAR *mQuery;
    };
}

#endif //R_EXASOL_ODBCSESSIONINFOIMPL_H
