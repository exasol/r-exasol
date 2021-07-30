#include <string>
#include <r_exasol/connection/protocol/meta_info_reader.h>
#include <r_exasol/connection/connection_exception.h>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

std::pair<std::string, uint16_t>  exa::metaInfoReader::read(exa::Socket& socket) {
    std::pair<std::string, uint16_t> hostInfo(std::string(), 0);

    struct { int32_t m; int32_t x; int32_t y:32; } proxyHeader = {0x02212102, 1, 1};
    struct { int32_t v; int32_t port; char s[16]; } proxyAnswer = {0, 0, ""};
    int r = 0;
    if ((r = socket.send((void*)&proxyHeader, sizeof(proxyHeader))) != sizeof(proxyHeader)) {
        std::stringstream stringStream;
        stringStream << "Failed to send proxy header (" << r << " != " << sizeof(proxyHeader) << ")";
        throw exa::ConnectionException (stringStream.str());
    }

    #ifndef _WIN32
        errno = 0;
    #endif

    if ((r = socket.recv((void*)&(proxyAnswer), sizeof(proxyAnswer))) != sizeof(proxyAnswer)) {
        std::stringstream stringStream;
        stringStream << "Failed to receive proxy header from " << socket.getConnectionInfo().first << ":" <<
                     socket.getConnectionInfo().second << " (" << r << " != " << sizeof(proxyAnswer) << "); errno: ";
    #ifndef _WIN32
        stringStream << errno;
    #else
        stringStream << WSAGetLastError();
    #endif
        throw exa::ConnectionException (stringStream.str());
    }
    proxyAnswer.s[15] = '\0';
    hostInfo.first = proxyAnswer.s;
    hostInfo.second = proxyAnswer.port;
    return hostInfo;
}
