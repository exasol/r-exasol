#include <r_exasol/websocket/exasol_response.h>
#include <r_exasol/websocket/exasol_exception.h>

namespace exa {

    boost::json::value parseResponse(const std::string& responseStr) {
        boost::json::value response;
        try {
            response = boost::json::parse(responseStr);
        } catch (const boost::system::system_error& e) {
            throw ExasolException(
                std::string("Failed to parse JSON response: ") + e.what(),
                "00000"
            );
        }

        if (!response.is_object()) {
            throw ExasolException("JSON response is not an object", "00000");
        }
        auto& obj = response.as_object();
        auto statusIt = obj.find("status");
        if (statusIt == obj.end()) {
            throw ExasolException("Response missing 'status' field", "00000");
        }

        const std::string status(statusIt->value().as_string());
        if (status == "ok") {
            return response;
        }

        std::string errorText = "Unknown error";
        std::string errorSqlCode = "00000";

        auto exceptionIt = obj.find("exception");
        if (exceptionIt != obj.end()) {
            auto& exObj = exceptionIt->value().as_object();
            auto textIt = exObj.find("text");
            if (textIt != exObj.end()) {
                errorText = std::string(textIt->value().as_string());
            }
            auto codeIt = exObj.find("sqlCode");
            if (codeIt != exObj.end()) {
                errorSqlCode = std::string(codeIt->value().as_string());
            }
        }

        throw ExasolException(errorText, errorSqlCode);
    }

} // namespace exa
