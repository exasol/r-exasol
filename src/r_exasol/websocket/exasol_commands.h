#ifndef R_EXASOL_WEBSOCKET_EXASOL_COMMANDS_H
#define R_EXASOL_WEBSOCKET_EXASOL_COMMANDS_H

#include <string>
#include <vector>
#include <cstdint>
#include <boost/json.hpp>
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
        boost::json::value data;

        ExecuteResult()
            : numResults(0),
              rowCount(0),
              resultSetHandle(-1),
              numColumns(0),
              numRows(0),
              numRowsInMessage(0) {}
    };

    class ExasolCommands {
    public:
        explicit ExasolCommands(WebSocketClient& ws);

        ExecuteResult execute(const std::string& sql);
        boost::json::value fetch(int resultSetHandle, int startPosition, int numBytes);
        void closeResultSet(int resultSetHandle);
        void disconnect();
        void setAttributes(const boost::json::value& attributes);
        boost::json::value getAttributes();

    private:
        WebSocketClient& mWs;
    };

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_EXASOL_COMMANDS_H
