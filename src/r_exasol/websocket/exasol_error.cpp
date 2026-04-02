#include <r_exasol/websocket/exasol_error.h>

#include <utility>

using json = nlohmann::json;

namespace exa {

    ExasolException::ExasolException(const std::string& message, std::string sqlCode)
        : std::runtime_error(message)
        , mSqlCode(std::move(sqlCode))
    {
    }

    const std::string& ExasolException::sqlCode() const {
        return mSqlCode;
    }

    json parseResponse(const std::string& responseStr) {
        json response;
        try {
            response = json::parse(responseStr);
        } catch (const json::parse_error& e) {
            throw ExasolException(
                std::string("Failed to parse JSON response: ") + e.what(),
                "00000"
            );
        }

        auto statusIt = response.find("status");
        if (statusIt == response.end()) {
            throw ExasolException("Response missing 'status' field", "00000");
        }

        const std::string status = statusIt->get<std::string>();
        if (status == "ok") {
            return response;
        }

        std::string errorText = "Unknown error";
        std::string errorSqlCode = "00000";

        auto exceptionIt = response.find("exception");
        if (exceptionIt != response.end()) {
            auto textIt = exceptionIt->find("text");
            if (textIt != exceptionIt->end()) {
                errorText = textIt->get<std::string>();
            }
            auto codeIt = exceptionIt->find("sqlCode");
            if (codeIt != exceptionIt->end()) {
                errorSqlCode = codeIt->get<std::string>();
            }
        }

        throw ExasolException(errorText, errorSqlCode);
    }

} // namespace exa
