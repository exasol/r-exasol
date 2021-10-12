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
        /**
         * Receive a buffer from the socket. The method blocks until the buffer is received.
         * @param buf The pointer of the buffer to receive the data.
         * @param len The size of the buffer.
         * @return -1 if an error occurred. Otherwise the size of the buffer which was received.
         */
        virtual ssize_t recv(void *buf, size_t len) = 0;
        /**
         * Send data to socket. The method is none-blocking.
         * @param buf Pointer to buffer to send.
         * @param len Length of the buffer to send.
         * @return -1 if an error occurred. Otherwise the length of the buffer which was sent.
         */
        virtual ssize_t send(const void *buf, size_t len) = 0;
        /**
         * Closes socket for sending.
         * All ongoing send operations will be finished, but future send operations on the socket
         * are not allowed.
         * @see https://www.baeldung.com/cs/sockets-close-vs-shutdown#shutdown
         */
        virtual void shutdownWr() = 0;
        /**
         * Closes socket for both sending/receiving.
         * All ongoing send/receive operations will be finished, but future send/receive operations on the socket
         * are not allowed.
         * @see https://www.baeldung.com/cs/sockets-close-vs-shutdown#shutdown
         */
        virtual void shutdownRdWr() = 0;
    };
}

#endif //R_EXASOL_SOCKET_H
