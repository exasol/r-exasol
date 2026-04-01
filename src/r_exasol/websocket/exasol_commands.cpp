#include <r_exasol/websocket/exasol_commands.h>
#include <r_exasol/websocket/exasol_error.h>

using json = nlohmann::json;

namespace exa {

    ExasolCommands::ExasolCommands(WebSocketClient& ws)
        : mWs(ws)
    {
    }

    ExecuteResult ExasolCommands::execute(const std::string& sql) {
        json cmd;
        cmd["command"] = "execute";
        cmd["sqlText"] = sql;

        std::string rawResponse = mWs.sendAndReceive(cmd.dump());
        json response = parseResponse(rawResponse);

        ExecuteResult result;
        result.numResults = 0;
        result.rowCount = 0;
        result.resultSetHandle = -1;
        result.numColumns = 0;
        result.numRows = 0;
        result.numRowsInMessage = 0;

        const auto& responseData = response.at("responseData");
        result.numResults = responseData.value("numResults", 0);

        if (result.numResults > 0) {
            const auto& results = responseData.at("results");
            const auto& firstResult = results.at(0);
            const std::string resultType = firstResult.value("resultType", "");

            if (resultType == "rowCount") {
                result.rowCount = firstResult.value("rowCount", int64_t(0));
            } else if (resultType == "resultSet") {
                const auto& resultSet = firstResult.at("resultSet");
                result.resultSetHandle = resultSet.value("resultSetHandle", -1);
                result.numColumns = resultSet.value("numColumns", 0);
                result.numRows = resultSet.value("numRows", int64_t(0));
                result.numRowsInMessage = resultSet.value("numRowsInMessage", int64_t(0));

                if (resultSet.contains("columns")) {
                    for (const auto& col : resultSet.at("columns")) {
                        result.columnNames.push_back(col.value("name", ""));
                        if (col.contains("dataType")) {
                            result.columnTypes.push_back(
                                col.at("dataType").value("type", ""));
                        } else {
                            result.columnTypes.push_back("");
                        }
                    }
                }

                if (resultSet.contains("data")) {
                    result.data = resultSet.at("data");
                }
            }
        }

        return result;
    }

    json ExasolCommands::fetch(int resultSetHandle, int startPosition, int numBytes) {
        json cmd;
        cmd["command"] = "fetch";
        cmd["resultSetHandle"] = resultSetHandle;
        cmd["startPosition"] = startPosition;
        cmd["numBytes"] = numBytes;

        std::string rawResponse = mWs.sendAndReceive(cmd.dump());
        json response = parseResponse(rawResponse);
        return response.at("responseData");
    }

    void ExasolCommands::closeResultSet(int resultSetHandle) {
        json cmd;
        cmd["command"] = "closeResultSet";
        cmd["resultSetHandles"] = json::array({resultSetHandle});

        std::string rawResponse = mWs.sendAndReceive(cmd.dump());
        parseResponse(rawResponse);
    }

    void ExasolCommands::disconnect() {
        json cmd;
        cmd["command"] = "disconnect";

        // Fire-and-forget: the server closes the connection after receiving
        // the disconnect command, so waiting for a response races with the
        // WebSocket Close frame and causes heap corruption on teardown.
        mWs.sendOnly(cmd.dump());
    }

    void ExasolCommands::setAttributes(const json& attributes) {
        json cmd;
        cmd["command"] = "setAttributes";
        cmd["attributes"] = attributes;

        std::string rawResponse = mWs.sendAndReceive(cmd.dump());
        parseResponse(rawResponse);
    }

    json ExasolCommands::getAttributes() {
        json cmd;
        cmd["command"] = "getAttributes";

        std::string rawResponse = mWs.sendAndReceive(cmd.dump());
        json response = parseResponse(rawResponse);
        return response.value("responseData", json::object());
    }

} // namespace exa
