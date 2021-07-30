#ifndef R_EXASOL_META_INFO_READER_H
#define R_EXASOL_META_INFO_READER_H

#include <r_exasol/connection/socket/socket.h>
#include <utility>
#include <cstdint>
#include <string>

namespace exa {
    namespace metaInfoReader {
        std::pair<std::string, uint16_t> read(Socket &);
    }
}


#endif //R_EXASOL_META_INFO_READER_H
