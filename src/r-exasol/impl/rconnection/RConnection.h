//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_RCONNECTION_H
#define R_EXASOL_RCONNECTION_H

#include <r-exasol/impl/rconnection/RconnFwd.h>
#include <Rdefines.h>
#include <r-exasol/if/Reader.h>

namespace exa {

    namespace rconnection {

        class RConnection {
        public:
            virtual ~RConnection() =default;

            virtual void release() = 0;
        };
    }
}

#endif //R_EXASOL_RCONNECTION_H
