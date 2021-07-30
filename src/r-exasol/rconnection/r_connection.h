#ifndef R_EXASOL_R_CONNECTION_H
#define R_EXASOL_R_CONNECTION_H

#include <r-exasol/rconnection/r_conn_fwd.h>
#include <Rdefines.h>
#include <r-exasol/connection/reader.h>

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
