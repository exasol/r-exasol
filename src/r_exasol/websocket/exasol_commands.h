#ifndef R_EXASOL_WEBSOCKET_EXASOL_COMMANDS_H
#define R_EXASOL_WEBSOCKET_EXASOL_COMMANDS_H

#include <string>
#include <vector>
#include <cstdint>
#include <r_exasol/external/nlohmann/json.hpp>
#include <r_exasol/websocket/websocket_client.h>

namespace exa {

    struct ExecuteResult {
        int numResults;
        int64_t rowCount;
        int resultSetHandle;
        int numColumns;
        int64_t numRows;
        int64_t numRowsInMessage;
        std::vector<std::string> columnNames;
        std::vector<std::string> columnTypes;
        nlohmann::json data;
    };

    class ExasolCommands {
    public:
        explicit ExasolCommands(WebSocketClient& client);

        ExecuteResult execute(const std::string& sql);
        nlohmann::json fetch(int resultSetHandle, int startPosition, int numBytes);
        void closeResultSet(int resultSetHandle);
        void disconnect();
        void setAttributes(const nlohmann::json& attributes);
        nlohmann::json getAttributes();

    private:
        WebSocketClient& mWs;
    };

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_EXASOL_COMMANDS_H
