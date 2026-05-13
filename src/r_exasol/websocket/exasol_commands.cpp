#include <r_exasol/websocket/exasol_commands.h>
#include <r_exasol/websocket/exasol_error.h>

namespace exa {

    ExasolCommands::ExasolCommands(WebSocketClient& ws)
        : mWs(ws)
    {
    }

    ExecuteResult ExasolCommands::execute(const std::string& sql) {
        boost::json::object cmd;
        cmd["command"] = "execute";
        cmd["sqlText"] = sql;

        std::string rawResponse = mWs.sendAndReceive(boost::json::serialize(cmd));
        boost::json::value response = parseResponse(rawResponse);

        ExecuteResult result;

        try {
            const auto& responseData = response.as_object().at("responseData").as_object();
            result.numResults = jsonValueOr(responseData, "numResults", 0);

            if (result.numResults > 0) {
                const auto& results = responseData.at("results").as_array();
                const auto& firstResult = results.at(0).as_object();
                const std::string resultType = jsonValueOr(firstResult, "resultType", std::string(""));

                if (resultType == "rowCount") {
                    result.rowCount = jsonValueOr(firstResult, "rowCount", int64_t(0));
                } else if (resultType == "resultSet") {
                    const auto& resultSet = firstResult.at("resultSet").as_object();
                    result.resultSetHandle = jsonValueOr(resultSet, "resultSetHandle", -1);
                    result.numColumns = jsonValueOr(resultSet, "numColumns", 0);
                    result.numRows = jsonValueOr(resultSet, "numRows", int64_t(0));
                    result.numRowsInMessage = jsonValueOr(resultSet, "numRowsInMessage", int64_t(0));

                    if (resultSet.contains("columns")) {
                        for (const auto& colVal : resultSet.at("columns").as_array()) {
                            const auto& col = colVal.as_object();
                            result.columnNames.push_back(jsonValueOr(col, "name", std::string("")));
                            if (col.contains("dataType")) {
                                result.columnTypes.push_back(
                                    jsonValueOr(col.at("dataType").as_object(), "type", std::string("")));
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
        } catch (const std::exception& e) {
            throw ExasolException(
                std::string("Malformed execute response: ") + e.what(), "00000");
        }

        return result;
    }

    boost::json::value ExasolCommands::fetch(int resultSetHandle, int startPosition, int numBytes) {
        boost::json::object cmd;
        cmd["command"] = "fetch";
        cmd["resultSetHandle"] = resultSetHandle;
        cmd["startPosition"] = startPosition;
        cmd["numBytes"] = numBytes;

        std::string rawResponse = mWs.sendAndReceive(boost::json::serialize(cmd));
        boost::json::value response = parseResponse(rawResponse);
        try {
            return response.as_object().at("responseData");
        } catch (const std::exception& e) {
            throw ExasolException(
                std::string("Malformed fetch response: ") + e.what(), "00000");
        }
    }

    void ExasolCommands::closeResultSet(int resultSetHandle) {
        boost::json::object cmd;
        cmd["command"] = "closeResultSet";
        boost::json::array handles;
        handles.push_back(resultSetHandle);
        cmd["resultSetHandles"] = handles;

        std::string rawResponse = mWs.sendAndReceive(boost::json::serialize(cmd));
        parseResponse(rawResponse);
    }

    void ExasolCommands::disconnect() {
        boost::json::object cmd;
        cmd["command"] = "disconnect";

        // Fire-and-forget: the server closes the connection after receiving
        // the disconnect command, so waiting for a response races with the
        // WebSocket Close frame and causes heap corruption on teardown.
        mWs.sendOnly(boost::json::serialize(cmd));
    }

    void ExasolCommands::setAttributes(const boost::json::value& attributes) {
        boost::json::object cmd;
        cmd["command"] = "setAttributes";
        cmd["attributes"] = attributes;

        std::string rawResponse = mWs.sendAndReceive(boost::json::serialize(cmd));
        parseResponse(rawResponse);
    }

    boost::json::value ExasolCommands::getAttributes() {
        boost::json::object cmd;
        cmd["command"] = "getAttributes";

        std::string rawResponse = mWs.sendAndReceive(boost::json::serialize(cmd));
        boost::json::value response = parseResponse(rawResponse);
        const auto& obj = response.as_object();
        auto it = obj.find("responseData");
        if (it == obj.end() || it->value().is_null()) {
            return boost::json::object{};
        }
        return it->value();
    }

} // namespace exa
