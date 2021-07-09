//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_RWRITERCONNECTION_H
#define R_EXASOL_RWRITERCONNECTION_H

#include "RconnFwd.h"
#include <Rdefines.h>
#include <impl/transfer/export/Writer.h>

namespace exa {
    namespace rconnection {
        class RWriterConnection {
        public:
            explicit RWriterConnection(writer::Writer * writer);

            SEXP create();

        private:
            writer::Writer * mWriter;
            Rconnection mConn;
        };
    }
}


#endif //R_EXASOL_RWRITERCONNECTION_H
