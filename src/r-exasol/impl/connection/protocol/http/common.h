//
// Created by thomas on 15/07/2021.
//

#ifndef R_EXASOL_HTTP_COMMON_H
#define R_EXASOL_HTTP_COMMON_H

#include <r-exasol/if/Socket.h>
#include <r-exasol/if/ConnectionException.h>

namespace exa {
    void readHttpHeader(Socket&);
}
#endif