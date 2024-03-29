/**
 * Definition of error callback functions.
 */

#ifndef R_EXASOL_ERROR_HANDLER_H
#define R_EXASOL_ERROR_HANDLER_H

#include <functional>
#include <string>

namespace exa {
    /**
     * Error function which used to indicate an error with detailed information (string parameter)
     */
    typedef std::function<void(std::string)> tErrorFunction;
    /**
     * Error function which might be called by the asynchronous remote execution.
     * <p>
     * Do not call any R functions (at least not such which use R's memory management) here!
     * <p>
     */
    typedef std::function<void()> tBackgroundAsyncErrorFunction;
}
#endif //R_EXASOL_ERROR_HANDLER_H
