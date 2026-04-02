#ifndef R_EXASOL_WEBSOCKET_EXASOL_ERROR_H
#define R_EXASOL_WEBSOCKET_EXASOL_ERROR_H

#include <stdexcept>
#include <string>
#include <r_exasol/external/nlohmann/json.hpp>

namespace exa {

    class ExasolException : public std::runtime_error {
    public:
        ExasolException(const std::string& message, std::string sqlCode);
        const std::string& sqlCode() const;
    private:
        std::string mSqlCode;
    };

    /// Parse a JSON response string from Exasol.
    /// Throws ExasolException if the response status is "error".
    nlohmann::json parseResponse(const std::string& responseStr);

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_EXASOL_ERROR_H
