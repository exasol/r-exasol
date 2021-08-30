#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/connection_exception.h>
#include <r_exasol/external/socket_api.h>

#include <sstream>
#include <r_exasol/debug_print/debug_printer.h>

#define SOCKET_STACK_PRINTER STACK_PRINTER( exa::SocketImpl);


exa::SocketImpl::SocketImpl()
: mSocket(-1) {}

ssize_t exa::SocketImpl::recv(void *buf, size_t len) {
    SOCKET_STACK_PRINTER;
    ssize_t retVal = 0;
#ifdef _WIN32
    retVal = ::recv(mSocket, static_cast<char*>(buf), len, MSG_WAITALL); //Winsocket returns SOCKET_ERROR = -1 in case of error! So this should be safe.
#else
    retVal = ::recv(mSocket, buf, len, MSG_WAITALL);
#endif
    return retVal;
}

ssize_t exa::SocketImpl::send(const void *buf, size_t len) {
    SOCKET_STACK_PRINTER;
#ifdef _WIN32
    return ::send(mSocket, static_cast<const char*>(buf), len, 0);
#else
    return ::send(mSocket, buf, len, 0);
#endif
}

void exa::SocketImpl::shutdownWr() {
    SOCKET_STACK_PRINTER;
    ::shutdown(mSocket, SHUT_WR);
    mSocket = -1;
}
void exa::SocketImpl::shutdownRdWr() {
    SOCKET_STACK_PRINTER;
    ::shutdown(mSocket, SHUT_RDWR);
    mSocket = -1;
}

void exa::SocketImpl::connect(const char *host, uint16_t port) {
    SOCKET_STACK_PRINTER;
    struct ::sockaddr_in serv_addr;
    struct ::hostent *server;

#ifdef _WIN32
    {
        WSADATA wsa;
        const int wsaResult = WSAStartup(MAKEWORD(2,0), &wsa);
	    if (wsaResult != 0) {
	        std::stringstream stringStream;
	        stringStream << "Could not initialize WSA: return value - " << wsaResult;
    	    throw ConnectionException(stringStream.str());
	    }
    }
#endif

    mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) {
        std::stringstream stringStream;
        stringStream << "Could not create socket: return value - " << mSocket << " errno - " << errno;
        throw ConnectionException(stringStream.str());
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

    int connection = ::connect(mSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (connection < 0) {
        std::stringstream stringStream;
        stringStream << "Could not connect to " << host << ":" << port
            << " return value:" << connection << " errno:" << errno;

#ifdef _WIN32
        stringStream << " (" << WSAGetLastError() << ")";
#endif
        throw ConnectionException(stringStream.str());
    }
}

exa::SocketImpl::~SocketImpl() {
    if(mSocket >= 0) {
        ::shutdown(mSocket, SHUT_RDWR);
    }
}

tSocket exa::SocketImpl::detach() {
    SOCKET_STACK_PRINTER;
    return std::exchange(mSocket, -1);
}
