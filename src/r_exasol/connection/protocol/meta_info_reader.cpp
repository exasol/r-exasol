#include <string>
#include <r_exasol/connection/protocol/meta_info_reader.h>
#include <r_exasol/connection/connection_exception.h>
#include <array>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

static constexpr int32_t kProxyHeaderMagic = 0x02212102;
static constexpr size_t kProxyAnswerHostSize = 16;
static constexpr size_t kProxyAnswerHostMaxIndex = kProxyAnswerHostSize - 1;

std::pair<std::string, uint16_t>  exa::metaInfoReader::read(exa::Socket& socket, const char *host, uint16_t port) {
    std::pair<std::string, uint16_t> hostInfo(std::string(), 0);

    struct { int32_t m; int32_t x; int32_t y:32; } proxyHeader = {kProxyHeaderMagic, 1, 1};
    struct { int32_t v; int32_t port; std::array<char, kProxyAnswerHostSize> s; } proxyAnswer = {0, 0, {}};
    ssize_t sendResult = socket.send((void*)&proxyHeader, sizeof(proxyHeader));
    if (sendResult != sizeof(proxyHeader)) {
        std::stringstream stringStream;
        stringStream << "Failed to send proxy header (" << sendResult << " != " << sizeof(proxyHeader) << ")";
        throw exa::ConnectionException (stringStream.str());
    }

    #ifndef _WIN32
        errno = 0;
    #endif

    ssize_t recvResult = socket.recv((void*)&(proxyAnswer), sizeof(proxyAnswer));
    if (recvResult != sizeof(proxyAnswer)) {
        std::stringstream stringStream;
        stringStream << "Failed to receive proxy header from " << host << ":" <<
                     port << " (" << recvResult << " != " << sizeof(proxyAnswer) << "); errno: ";
    #ifndef _WIN32
        stringStream << errno;
    #else
        stringStream << WSAGetLastError();
    #endif
        throw exa::ConnectionException (stringStream.str());
    }
    proxyAnswer.s[kProxyAnswerHostMaxIndex] = '\0';
    hostInfo.first = proxyAnswer.s.data();
    hostInfo.second = proxyAnswer.port;

    return hostInfo;
}
