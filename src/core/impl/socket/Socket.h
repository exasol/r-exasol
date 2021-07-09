//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_SOCKET_H
#define R_EXASOL_SOCKET_H

#include <cstdio>

namespace exa {
    class Socket {
    public:
        virtual size_t recv(void *buf, size_t len) = 0;
        virtual ssize_t send(const void *buf, size_t len) = 0;
        virtual void shutdownWr() = 0;
        virtual void shutdownRdWr() = 0;
    };
}

#endif //R_EXASOL_SOCKET_H
