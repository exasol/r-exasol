//
// Created by thomas on 16/07/2021.
//

#ifndef R_EXASOL_ERRORHANDLER_H
#define R_EXASOL_ERRORHANDLER_H

#include <functional>
#include <string>

namespace exa {
    typedef std::function<void(std::string)> tErrorFunction;
    typedef std::function<void()> tBackgroundOdbcErrorFunction;
}
#endif //R_EXASOL_ERRORHANDLER_H
