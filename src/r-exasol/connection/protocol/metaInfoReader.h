//
// Created by thomas on 16/07/2021.
//

#ifndef R_EXASOL_METAINFOREADER_H
#define R_EXASOL_METAINFOREADER_H

#include <r-exasol/connection/socket/Socket.h>
#include <utility>
#include <cstdint>
#include <string>

namespace exa {
    namespace metaInfoReader {
        std::pair<std::string, uint16_t> read(Socket &);
    }
}


#endif //R_EXASOL_METAINFOREADER_H
