#ifndef R_EXASOL_SOCKET_H
#define R_EXASOL_SOCKET_H

#include <cstdio>
#include <cstdint>
#include <utility>
#include <string>

namespace exa {
    /**
     * Abstract interface for reading/writing to a tcp socket.
     */
    class Socket {
    public:
        virtual ~Socket() = default;
        virtual void connect(const char * host, uint16_t port) = 0;
        virtual size_t recv(void *buf, size_t len) = 0;
        virtual ssize_t send(const void *buf, size_t len) = 0;
        virtual void shutdownWr() = 0;
        virtual void shutdownRdWr() = 0;
        virtual std::pair<std::string, uint16_t > getConnectionInfo() const = 0;
    };
}

#endif //R_EXASOL_SOCKET_H
