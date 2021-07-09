//
// Created by thomas on 08/07/2021.
//

#include <impl/socket/SocketImpl.h>

#ifndef _WIN32
#include <sys/socket.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif
#endif

exa::SocketImpl::SocketImpl(tSocket socket)
: mSocket(socket) {}

size_t exa::SocketImpl::recv(void *buf, size_t len) {
    return ::recv(mSocket, buf, len, MSG_WAITALL);
}

ssize_t exa::SocketImpl::send(const void *buf, size_t len) {
    return ::send(mSocket, buf, len, 0);
}

void exa::SocketImpl::shutdownWr() {
    ::shutdown(mSocket, SHUT_WR);
    mSocket = -1;
}
void exa::SocketImpl::shutdownRdWr() {
    ::shutdown(mSocket, SHUT_RDWR);
    mSocket = -1;
}