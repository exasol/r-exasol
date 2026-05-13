#include <r_exasol/websocket/exasol_exception.h>

namespace exa {

    ExasolException::ExasolException(const std::string& message, const std::string& sqlCode)
        : std::runtime_error(message)
        , mSqlCode(sqlCode)
    {
    }

    const std::string& ExasolException::sqlCode() const {
        return mSqlCode;
    }

} // namespace exa
