#ifndef R_EXASOL_WEBSOCKET_EXASOL_EXCEPTION_H
#define R_EXASOL_WEBSOCKET_EXASOL_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace exa {

    class ExasolException : public std::runtime_error {
    public:
        ExasolException(const std::string& message, const std::string& sqlCode);
        const std::string& sqlCode() const;
    private:
        std::string mSqlCode;
    };

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_EXASOL_EXCEPTION_H
