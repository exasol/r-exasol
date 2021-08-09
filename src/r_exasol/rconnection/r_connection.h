#ifndef R_EXASOL_R_CONNECTION_H
#define R_EXASOL_R_CONNECTION_H

#include <r_exasol/external/r_conn_fwd.h>
#include <r_exasol/external/r.h>
#include <r_exasol/connection/reader.h>

namespace exa {

    namespace rconnection {

        /**
         * Abstract interface for a r-connection.
         */
        class RConnection {
        public:
            virtual ~RConnection() =default;

            /**
             * Releases resources of r-connection.
             */
            virtual void release() = 0;
        };
    }
}

#endif //R_EXASOL_R_CONNECTION_H
