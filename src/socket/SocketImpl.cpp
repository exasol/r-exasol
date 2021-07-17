//
// Created by thomas on 08/07/2021.
//

#include <socket/SocketImpl.h>

#include <if/ConnectionException.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <netdb.h>
#include <sstream>

#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif
#endif

exa::SocketImpl::SocketImpl()
: mSocket(-1) {}

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

void exa::SocketImpl::connect(const char *host, uint16_t port) {
    mConnectionInfo = std::make_pair(std::string(host), port);
    struct sockaddr_in serv_addr;
    struct hostent *server;

#ifdef _WIN32
    {
        WSADATA wsa;
	    if (WSAStartup(MAKEWORD(2,0), &wsa) != 0) {
    	    throw ConnectionException("Could not initialize WSA");
	    }
    }
#endif

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) {
        throw ConnectionException("Could not create socket.");
    }

    ::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    server = ::gethostbyname(host);
    if (server == nullptr) {
        std::stringstream stringStream;
        stringStream << "No such host: " << host;
        throw ConnectionException(stringStream.str());
    }
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    if (::connect(mSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::stringstream stringStream;
        stringStream << "Could not connect to " << host << ":" << port;

#ifdef _WIN32
        stringStream << " (" << WSAGetLastError() << ")"
#endif
        throw ConnectionException(stringStream.str());
    }
}
