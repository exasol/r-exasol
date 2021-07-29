#ifndef R_EXASOL_RCONNECTION_H
#define R_EXASOL_RCONNECTION_H

#include <r-exasol/rconnection/RconnFwd.h>
#include <Rdefines.h>
#include <r-exasol/connection/Reader.h>

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

#endif //R_EXASOL_RCONNECTION_H
