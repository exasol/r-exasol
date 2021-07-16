//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_SOCKETIMPL_H
#define R_EXASOL_SOCKETIMPL_H

#include <if/Socket.h>
#include <if/ExaTypes.h>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <string>

namespace exa {
    class SocketImpl : public Socket {
    public:
        explicit SocketImpl();

        void connect(const char* host, uint16_t port) override;
        size_t recv(void *buf, size_t len) override;
        ssize_t send(const void *buf, size_t len) override;
        void shutdownWr() override;
        void shutdownRdWr() override;

        std::pair<std::string, uint16_t> getConnectionInfo() const override { return mConnectionInfo; }
    private:
        tSocket mSocket;
        std::pair<std::string, uint16_t> mConnectionInfo;
    };
}

#endif //R_EXASOL_SOCKETIMPL_H
