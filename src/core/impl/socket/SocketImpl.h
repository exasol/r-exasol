//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_SOCKETIMPL_H
#define R_EXASOL_SOCKETIMPL_H

#include <impl/socket/Socket.h>
#include <ExaTypes.h>
#include <cstdio>

namespace exa {
    class SocketImpl : public Socket {
    public:
        explicit SocketImpl(tSocket socket);

        size_t recv(void *buf, size_t len) override;
        ssize_t send(const void *buf, size_t len) override;
        void shutdownWr() override;
        void shutdownRdWr() override;

    private:
        tSocket mSocket;
    };
}

#endif //R_EXASOL_SOCKETIMPL_H
