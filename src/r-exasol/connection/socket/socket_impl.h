#ifndef R_EXASOL_SOCKET_IMPL_H
#define R_EXASOL_SOCKET_IMPL_H

#include <r-exasol/connection/socket/socket.h>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <string>


#ifdef _WIN32
#include <winsock2.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
typedef int tSocket;
#else
typedef SOCKET tSocket;
#endif

#ifdef __cplusplus
}
#endif

namespace exa {
    /**
     * Socket implementation to read/write from/to a TCP socket.
     */
    class SocketImpl : public Socket {
    public:
        explicit SocketImpl();
        ~SocketImpl() override;
        void connect(const char* host, uint16_t port) override;
        size_t recv(void *buf, size_t len) override;
        ssize_t send(const void *buf, size_t len) override;
        void shutdownWr() override;
        void shutdownRdWr() override;

        /**
         * Returns connection info which was used to open the socket.
         * @return Hostname/Port number.
         */
        std::pair<std::string, uint16_t> getConnectionInfo() const override { return mConnectionInfo; }
    private:
        tSocket mSocket;
        std::pair<std::string, uint16_t> mConnectionInfo;
    };
}

#endif //R_EXASOL_SOCKET_IMPL_H
