//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_RREADERCONNECTION_H
#define R_EXASOL_RREADERCONNECTION_H

#include "RconnFwd.h"
#include <Rdefines.h>
#include <impl/transfer/import/Reader.h>

namespace exa {

    namespace rconnection {

        class RReaderConnection {
        public:
            explicit RReaderConnection(import::Reader * reader);

            SEXP create();

        private:
            import::Reader * mReader;
            Rconnection mConn;
        };
    }
}

#endif //R_EXASOL_RREADERCONNECTION_H
