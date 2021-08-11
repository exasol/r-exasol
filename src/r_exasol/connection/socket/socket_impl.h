#ifndef R_EXASOL_SOCKET_IMPL_H
#define R_EXASOL_SOCKET_IMPL_H

#include <r_exasol/connection/socket/socket.h>
#include <r_exasol/external/socket_fwd.h>
#include <r_exasol/debug_print/debug_printer.h>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <string>
#include <fstream>


namespace exa {
    /**
     * Socket implementation to read/write from/to a TCP socket.
     */
    class SocketImpl : public Socket {
    public:
        explicit SocketImpl();
        ~SocketImpl() override;
        void connect(const char* host, uint16_t port);
        ssize_t recv(void *buf, size_t len) override;
        ssize_t send(const void *buf, size_t len) override;
        void shutdownWr() override;
        void shutdownRdWr() override;

        tSocket detach();

    private:
        ObjectLifecycleLogger<SocketImpl> mLifecycleLogger;
        tSocket mSocket;
    };
}

#endif //R_EXASOL_SOCKET_IMPL_H
