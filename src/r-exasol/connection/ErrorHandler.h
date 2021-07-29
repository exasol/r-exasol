/**
 * Definition of error callback functions.
 */

#ifndef R_EXASOL_ERRORHANDLER_H
#define R_EXASOL_ERRORHANDLER_H

#include <functional>
#include <string>

namespace exa {
    /**
     * Error function which used to indicate an error with detailed information (string parameter)
     */
    typedef std::function<void(std::string)> tErrorFunction;
    /**
     * Error function which might be called by the asynchronous ODBC statement execution.
     */
    typedef std::function<void()> tBackgroundOdbcErrorFunction;
}
#endif //R_EXASOL_ERRORHANDLER_H
