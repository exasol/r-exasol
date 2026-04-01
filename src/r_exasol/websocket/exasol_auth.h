#ifndef R_EXASOL_WEBSOCKET_EXASOL_AUTH_H
#define R_EXASOL_WEBSOCKET_EXASOL_AUTH_H

#include <string>
#include <cstdint>
#include <r_exasol/websocket/websocket_client.h>

namespace exa {

    struct LoginResponse {
        int64_t sessionId;
        int protocolVersion;
        std::string releaseVersion;
        std::string databaseName;
        std::string productName;
        int maxDataMessageSize;
        int maxIdentifierLength;
        int maxVarcharLength;
        std::string identifierQuoteString;
        std::string timeZone;
        std::string timeZoneBehavior;
    };

    class ExasolAuth {
    public:
        /// Encrypt password using RSA public key (PKCS#1 v1.5, Base64 encoded).
        static std::string encryptPassword(
            const std::string& password,
            const std::string& publicKeyModulus,
            const std::string& publicKeyExponent);

        /// Perform the full Exasol login sequence over WebSocket.
        static LoginResponse login(
            WebSocketClient& ws,
            const std::string& username,
            const std::string& password,
            int protocolVersion = 3);
    };

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_EXASOL_AUTH_H
